#!/usr/bin/env perl
use strict;
use warnings;

use v5.014;

###
# A Twitter bot to post a gallery, one pic at a time, every N hours

# fudge @INC
use FindBin qw( $RealBin );
use lib $RealBin;

use Twitter::API;

use File::Basename;
use Scalar::Util 'blessed';

### Helper Functions
# Recursively read a directory and add results to an array.
sub rec_read_dir 
{
  my $d = shift;

  my @files;

  opendir(my $dh, $d) || die "Can't open directory $d: $!\n";
  while (readdir $dh) {
    next if substr($_,0,1) eq '.';

    my $path = $d . '/' . $_;
    if (-d $path) {
      push @files, rec_read_dir($path);
    } elsif (-f $path) {
      if ($path =~ m/\.(pud)$/i) {
        push @files, $path;
      }
    }
  }
  closedir($dh);

  return @files;
}

###############################################################
### CODE
###############################################################
# Go read the config file
my %config = do "$RealBin/config.pl"
  or die "Couldn't read config.pl: $! $@";

# Go find a map
my @files = rec_read_dir('maps');

my $file = $files[rand @files];

my $cmd = './pud2bmp -x "' . $file . '"';
print $cmd . "\n";
my $result = `$cmd`;

my $bmpname = $file . '.bmp';

my $pngname = $file . '.png';
unlink($pngname);

#my $optipng = 'optipng -o7 -strip all -zm1-9 ' . $file . '.bmp';
my $optipng = 'optipng -o1 -strip all "' . $bmpname . '"';
print $optipng . "\n";
`$optipng`;

unlink($bmpname);

# delete the BMP it's huge

eval {
  # Connect to Twitter
  my $client = Twitter::API->new_with_traits(
    traits          => qw( NormalizeBooleans DecodeHtmlEntities RetryOnError ),
    consumer_key    => $config{consumer_key},
    consumer_secret => $config{consumer_secret},
    access_token    => $config{access_token},
    access_token_secret => $config{access_token_secret},
  );

  ###
  # READY TO POST!!
  # Upload media image
  my $upload_return_object = $client->post('https://upload.twitter.com/1.1/media/upload.json', {
    media_category => 'tweet_image',
    media => [ $pngname ], #'image.png', Content_Type => "image/png" ]
  });

  # Compose tweet.
  #my $post = $result;
  # Post!
#  $nt->update({status => $post, media_ids => $upload_return_object->{media_id}});
  # post

  my ($part1, $part2) = split /\n\n/, $result, 2;

  my $r = $client->post('statuses/update', {
      status    => $part1,
      media_ids => $upload_return_object->{media_id},
  #    in_reply_to_status_id        => $last_id,
  #    auto_populate_reply_metadata => 'true'
  });
  my $last_id = $r->{id_str};

  # followup with details
  $client->post('statuses/update', {
      status                       => $part2,
      in_reply_to_status_id        => $last_id,
      auto_populate_reply_metadata => 'true'
  });
};

# error handling
if ( my $err = $@ ) {
  die $@ unless blessed $err && $err->isa('Net::Twitter::Lite::Error');

  warn "HTTP Response Code: ", $err->code, "\n",
       "HTTP Message......: ", $err->message, "\n",
       "Twitter error.....: ", $err->error, "\n";
}
