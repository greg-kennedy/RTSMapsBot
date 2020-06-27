package Common;
use strict;
use warnings;

###############################################################################
# COMMON functions that might be of use to any decoder
###############################################################################

use Carp qw(croak);

use File::Temp qw( tempfile );

use autodie qw(:all);

use Exporter 'import'; # gives you Exporter's import() method directly
our @EXPORT = qw(ci_path read_bytes);  # symbols to export on request

###############################################################################
sub readdir_recursive {
  my $path = shift;

  opendir(my $dh, $path);
  my @items = readdir($dh);
  closedir $dh;

  my @ret;
  foreach my $entry (@items) {
    if ($entry eq '.' || $entry eq '..') { next }
    my $filename = $path . '/' . $entry;
    if (-d $filename) { push @ret, readdir_recursive($filename) }
    else { push @ret, $filename }
  }
  return @ret;
}

# case-insensitive file open
#  throws an error if more than one file matches the casing
my %ci_cache;
sub ci_path
{
  my $path = lc(+shift);

  if (! exists $ci_cache{$path})
  {
    my $real_path = '.';
    foreach my $part ( split /\//, $path ) {
      opendir(my $dh, $real_path);
      my @items = grep { lc($_) eq $part } readdir($dh);
      closedir $dh;

      if (@items != 1) {
        die "Error handling path part '$part': found @items items";
      } else {
        # grow the "real" path
        $real_path = $real_path . '/' . $items[0];
      }
    }

    $ci_cache{$path} = $real_path;
  }

   return $ci_cache{$path};
}

sub read_bytes {
  my ($fp, $count) = @_;
  my $buffer;

  my $bytes_read = read $fp, $buffer, $count;
  if (!defined $bytes_read) {
    croak "Read ($bytes_read) failed on $fp: $!";
  } elsif ($bytes_read == 0) {
    croak "Read ($bytes_read) failed on $fp: at end of file";
  } elsif ($bytes_read != $count) {
    croak "Read ($bytes_read) failed on $fp: got $bytes_read";
  }

  return $buffer;
}

# write a temp TGA file using a palette
#  returns a filename to the temp file
# turns out this is pretty useless since OptiPNG can't read .tga
=pod
sub save_tga {
  my ($palette, $image) = @_;
  my $height = @$image;
  my $width = @{$image->[0]};

  # get us a file to write to
  my ($fh, $filename) = tempfile( 'X' x 12, DIR => '/tmp', SUFFIX => '.tga' );

  # write header: colormap image, N palette entries, 24bits per entry, and top-to-bottom
  print $fh pack('CCCvvCvvvvCC', 0, 1, 1, 0, scalar(@$palette), 24, 0, 0, $width, $height, 8, 32);
  foreach my $color (@$palette)
  {
    print $fh pack('CCC', @$color);
  }
  for (my $y = 0; $y < $height; $y ++) {
    print $fh pack('C*', @{$image->[$y]});
  }
  close $fh;

  return $filename;
}
=cut

# write a temp BMP file using a palette
#  returns a filename to the temp file
sub save_bmp {
  my ($palette, $image) = @_;
  my $height = @$image;
  my $width = @{$image->[0]};

  # get us a file to write to
  my ($fh, $filename) = tempfile( 'X' x 12, DIR => '/tmp', SUFFIX => '.bmp' );

  # make some estimates for the BMP sizes

  print $fh pack('nVvvV', 0x424D, 14 + 40 + 1024 + ($height * $width), 0, 0, 14 + 40 + 1024);
  print $fh pack('VVVvvVVVVVV', 40, $width, $height, 1, 8, 0, 0, 72, 72, 256, 0);
  foreach my $color (@$palette)
  {
    print $fh pack('CCCx', $color->[2], $color->[1], $color->[0]);
  }
  for (my $y = $height - 1; $y >= 0; $y --) {
    print $fh pack('C*', @{$image->[$y]});
  }
  close $fh;

  return $filename;
}

# write a temp PPM file using a palette
#  returns a filename to the temp file
# use the BMP function, it's faster :P
=pod
sub save_ppm {
  my ($palette, $image) = @_;
  my $height = @$image;
  my $width = @{$image->[0]};

  # get us a file to write to
  my ($fh, $filename) = tempfile( 'X' x 12, DIR => '/tmp', SUFFIX => '.ppm' );
  print $fh "P6\n$width $height\n255\n";
  for (my $y = 0; $y < $height; $y ++) {
    for (my $x = 0; $x < $width; $x ++) {
      print $fh pack('C[3]', @{$palette->[$image->[$y][$x]]});
    }
  }
  close $fh;

  return $filename;
}
=cut

# converts an image to png
sub optipng {
    my $infile = shift;

    my $outfile = $infile; $outfile =~ s/\....$/.png/;
    # delete existing png if exists
    if (-e $outfile) { unlink $outfile }

    #my $optipng = 'optipng -o7 -strip all -zm1-9 "' . $infile . '"';
    my $optipng = 'optipng -o2 -strip all "' . $infile . '"';
    system($optipng);

    # clean up old file
    unlink($infile);

    return $outfile;
}

1;
