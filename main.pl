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
use Module::Load;

# local helpers
use Common;

###############################################################
### CODE
###############################################################
# Go read the config file
my %config = do "$RealBin/config.pl"
  or die "Couldn't read config.pl: $! $@";

# Go find a map
my @files;
if ($ARGV[0]) { @files = ( $ARGV[0] ) }
else { @files = Common::readdir_recursive('maps') }

my $result;
while (! $result) {
  # pick a map
  my $file = $files[rand @files];

  # try a decode
  if ($file =~ m/\.pud$/i) {
    load 'Decoder::Warcraft2';
    $result = Decoder::Warcraft2::render($file);
  } elsif ($file =~ m/\.ota$/i) {
    load 'Decoder::TotalAnnihilation';
    $result = Decoder::TotalAnnihilation::render($file);
  }
}

# try to make a png first
my $upload_file = Common::optipng($result->{data});

# Twitter rejects uploads over 5mb, so if the png is too large, try making a jpeg instead
if (-s $upload_file > 5_000_000) {
  unlink($upload_file);
  print STDERR "File too large, attempting shrink.\n";
  my $new_bmp = Common::shrink_bmp( $result->{data} );
  unlink($result->{data}); $result->{data} = $new_bmp;

  print STDERR "Calling optipng on new file.\n";
  $upload_file = Common::optipng($result->{data});
  #$upload_file = $result->{data} . '.jpg';
  #`convert -define registry:temporary-path=/var/tmp $result->{data} -resize 4096x4096 $upload_file`;
}

my $q = 100;
while (-s $upload_file > 5_000_000) {
  print STDERR "Rejecting $upload_file because it is >5M (" . (-s $upload_file) . ")\n";
  unlink($upload_file);
  $upload_file = Common::cjpeg($result->{data}, $q);
  $q -= 5;
}
print STDERR "Uploading $upload_file (" . (-s $upload_file) . ")\n";

eval {
  # Connect to Twitter
  my $client = Twitter::API->new_with_traits(
    traits          => [ qw( NormalizeBooleans DecodeHtmlEntities RetryOnError ) ],
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
    media => [ $upload_file ], #'image.png', Content_Type => "image/png" ]
  });

  # Compose tweet.
  #my $post = $result;
  # Post!
#  $nt->update({status => $post, media_ids => $upload_return_object->{media_id}});
  # post

  my $r = $client->post('statuses/update', {
      status    => $result->{text},
      media_ids => $upload_return_object->{media_id},
  #    in_reply_to_status_id        => $last_id,
  #    auto_populate_reply_metadata => 'true'
  });
  my $last_id = $r->{id_str};

  my @remaining = split /\n/, $result->{extra};
  while (@remaining) {
    my $m = '';
    while (@remaining && length($m . "\n" . $remaining[0]) < 280) {
      $m = $m . "\n" . shift(@remaining);
    }

    # followup with details
    $r = $client->post('statuses/update', {
        status                       => $m,
        in_reply_to_status_id        => $last_id,
        auto_populate_reply_metadata => 'true'
    });
    $last_id = $r->{id_str};
  }
};

# error handling
if ( my $err = $@ ) {
  die $@ unless blessed $err && $err->isa('Net::Twitter::Lite::Error');

  warn "HTTP Response Code: ", $err->code, "\n",
       "HTTP Message......: ", $err->message, "\n",
       "Twitter error.....: ", $err->error, "\n";
}

# clean up
unlink( $upload_file );
unlink( $result->{data} );
