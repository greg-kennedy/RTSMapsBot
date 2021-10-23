package Decoder::Warcraft2;
use strict;
use warnings;

use autodie qw(:all);

# render a map
sub render {
    my $file = shift;

    # warcraft 2 maps are handled by an external tool
    my $cmd = './pud2bmp -x "' . $file . '"';
    #print $cmd . "\n";
    my $result = `$cmd`;
    print $result . "\n";

    my $bmp = $file . '.bmp';
    #if (DEBUG) { print STDERR "Calling optipng...\n" }
#    my $png = Common::optipng( $bmp );

    # split the output text into some parts
    my ($part1, $part2) = split /\n\n/, $result, 2;
    $part1 =~ s/maps\/war2\///;
    $part1 = "WARCRAFT II\n" . $part1;

    # return info
    return {
      type => 'bmp',
      data => $bmp,
      text => $part1,
      extra => $part2
    };
}

1;
