package Decoder::TotalAnnihilation;
use strict;
use warnings;

use autodie;

use Fcntl qw( SEEK_SET );
use List::Util qw( min uniqstr );

use Common;

use constant DEBUG => 1;

###############################################################################
# read palette from palette file
#  palette is RGB0 x 256
sub _pal {
  my $filename = shift;

  my @palette;
  open (my $fp, '<:raw', $filename);
  while (! eof($fp)) {
    push @palette, [ unpack('C[3]x', read_bytes($fp, 4)) ];
  }
  close $fp;

  return \@palette;
}

###############################################################################
# read alphablend table from alp file
#  table is 256 x 256
sub _alp {
  my $filename = shift;

  my @table;
  open (my $fp, '<:raw', $filename);
  while (! eof($fp)) {
    push @table, [ unpack('C[256]', read_bytes($fp, 256)) ];
  }
  close $fp;

  return \@table;
}
###############################################################################
# Total Annihilation data file (tdf) - also .ota, etc
sub _tdf_rec {
  my $fp = shift;

  my %tdf;
  while (my $line = <$fp>) {
    # strip comments
    $line =~ s/\/\/.*$//g;
    # trim whitespace
    $line =~ s/^\s+//g;
    $line =~ s/\s+$//g;
    # parse line
    if ($line eq '') { next }
    elsif ($line eq '{') { next }
    elsif ($line eq '}') { return \%tdf }
    elsif ($line =~ m/^\[([^]]+)\]$/) {
      my $name = lc($1);
      $tdf{$name} = _tdf_rec($fp);
    }
    elsif ($line =~ m/^([^=]+?)\s*=\s*([^;]*?)\s*;?$/) {
      $tdf{lc($1)} = $2;
    } else {
      warn "Unable to parse TDF line '$line'";
    }
  }
  return \%tdf;
}

# main entry point for TDF reading
sub _tdf {
  my $filename = shift;

  # ok now the file
  open(my $fp, '<', $filename);
  my $tdf = _tdf_rec($fp);
  close $fp;

  return $tdf;
}

###############################################################################
# the main TNT file - objects, tilemaps, etc
sub _tnt {
  my $filename = shift;

  my %tnt;

  # ok now the file
  open(my $fp, '<:raw', $filename);

  # read tnt header
  #my ( $IDVersion, $Width, $Height, $PtrMapData, $PtrMapAttr, $PtrTileGfx, $Tiles, $TileAnims, $PtrTileAnims, $SeaLevel, $PtrMiniMap, $unknown1, $pad1, $pad2, $pad3, $pad4 ) = unpack('V[10]', read_bytes($fp, 0x40));
  my ( $Width, $Height, $PtrMapData, $PtrMapAttr, $PtrTileGfx, $Tiles, $TileAnims, $PtrTileAnims, $SeaLevel, $PtrMiniMap ) = unpack('x[4]V[10]', read_bytes($fp, 44));

  $tnt{width} = $Width;
  $tnt{height} = $Height;

  my $TilesW = $Width / 2;
  my $TilesH = $Height / 2;
  my $TilesCount = $TilesW * $TilesH;

  # read tile indices
  seek($fp, $PtrMapData, SEEK_SET);
  @{$tnt{mapData}} = unpack("v[$TilesCount]", read_bytes($fp, $TilesCount * 2));

  # read tile attributes
  seek($fp, $PtrMapAttr, SEEK_SET);
  for my $y (0 .. $Height - 1) {
    for my $x (0 .. $Width - 1) {
      ($tnt{mapHeight}[$y][$x], $tnt{mapFeature}[$y][$x]) = unpack("Cvx", read_bytes($fp, 4));
    }
  }

  # read map feature items
  seek($fp, $PtrTileAnims, SEEK_SET);
  $tnt{anims} = [];
  for (1 .. $TileAnims) {
  #for (my $i = 0; $i < $TileAnims; $i ++) {
    push @{$tnt{anims}}, lc(unpack("x[4]Z[128]", read_bytes($fp, 132)));
  }

  # read tile gfx
  seek($fp, $PtrTileGfx, SEEK_SET);
#  for (my $i = 0; $i < $Tiles; $i ++) {
  for my $i (0 .. $Tiles - 1) {
#    for (my $y = 0; $y < 32; $y ++) {
    for (0 .. 31) {
      #$tnt{tileGfx}[$i][$y] = [ unpack('C[32]', read_bytes($fp, 32)) ];
      push @{$tnt{tileGfx}[$i]}, [ unpack('C[32]', read_bytes($fp, 32)) ];
    }
  }

  # think we're done with the file here
  close($fp);

  return \%tnt;
}

###############################################################################
# Graphics (sprite, animation) file
# NOTE: This is not a full gaf decoder - that takes too long :P
#  It can extract only the first frame of a single entry.
sub _gaf {
  my $filename = shift;
  my $entryname = lc(+shift);

  # ok now the file
  open(my $fp, '<:raw', $filename);

  # read header
  my ($IDVersion, $Entries, $Unknown1) = unpack('V[3]', read_bytes($fp, 12));

  # pointers to each frameset in the file
  my @pointers = unpack('V*', read_bytes($fp, 4 * $Entries));

  foreach my $pointer (@pointers) {
    seek($fp, $pointer, SEEK_SET);
    my ($Frames, $Unknown2, $Unknown3, $Name) = unpack('vvVZ[32]', read_bytes($fp, 40));
    next if (lc($Name) ne $entryname);

    #if ($Frames == 0) { next }
    # so... we SHOULD unpack all frames here, but this still image will only use the first
    my ($PtrFrameTable, $Unknown4) = unpack('VV', read_bytes($fp, 8));
    # more frame table entries would follow here

    # go to the data for the 0th frame
    seek($fp, $PtrFrameTable, SEEK_SET);
    my ($Width, $Height, $XPos, $YPos, $TransparentIndex, $Compressed, $FramePointers, $PtrFrameData) = unpack('vvs<s<CCvx[4]Vx[4]', read_bytes($fp, 24));

    my %gaf;
    $gaf{width} = $Width;
    $gaf{height} = $Height;
    $gaf{xPos} = $XPos;
    $gaf{yPos} = $YPos;
    $gaf{transparentIndex} = $TransparentIndex;

    if ($FramePointers) {
      die "unsupported ($Name) :(";
    } else {
      seek($fp, $PtrFrameData, SEEK_SET);
      if (! $Compressed) {
        for (0 .. $Height - 1) {
          push @{$gaf{data}}, [ unpack('C*', read_bytes($fp, $Width)) ]
        }
      } else {
        for (my $y = 0; $y < $Height; $y ++) {
          # RLE compressed, with transparency
          my $l = unpack('v', read_bytes($fp, 2));
          my $x = 0;
          my $i = 0;
          $gaf{data}[$y] = [];
          while ($i < $l) {
            my $c = unpack('C', read_bytes($fp, 1)); $i ++;
            if ($c & 0x01) { $c = $c >> 1; $x += $c; }
            elsif ($c & 0x02) {
              $c = ($c >> 2) + 1;
              my $b = unpack('C', read_bytes($fp, 1));
              $i ++;
              while ($c) {
                $gaf{data}[$y][$x] = $b;
                $x ++; $c --;
              }
            } else {
              $c = ($c >> 2) + 1;
              while ($c) {
                $gaf{data}[$y][$x] = unpack('C', read_bytes($fp, 1));
                $i ++;
                $x ++; $c --;
              }
            }
          }
        }
      }
    }

    return \%gaf;
  }

  die "Error: request for entry $entryname in file $filename but it was not found";
}

###############################################################################
# render a map
sub render {
  my $filename = shift;
  if (DEBUG) { print STDERR "TA: \"$filename\"\n" }

  # SETUP - common reads
  # read TA palette from file
  if (DEBUG) { print STDERR "Read palette...\n" }
  my $pal = _pal(ci_path('totala/palettes/PALETTE.PAL'));

  # pre-calculate a table of 50/50 alpha blends
  if (DEBUG) { print STDERR "Read blend table...\n" }
  my $alp = _alp(ci_path('totala/palettes/PALETTE.ALP'));

  # read all Feature definitions
  if (DEBUG) { print STDERR "Reading feature defs...\n" }
  my %tdf;
  foreach (Common::readdir_recursive('totala/features')) {
    if (DEBUG) { print STDERR " . $_\n" }
    %tdf = (%tdf, %{_tdf($_)});
  }

  # read mission textfile
  if (DEBUG) { print STDERR "Reading ota file...\n" }
  my $ota = _tdf(ci_path($filename));

  # read terrain map
  if (DEBUG) { print STDERR "Reading tnt file...\n" }
  my $tntname = $filename;
  $tntname =~ s/\.ota$/.tnt/i;
  my $tnt = _tnt(ci_path($tntname));

  # Extract "features" from tnt and ota into one large list.
  if (DEBUG) { print STDERR "Examining tnt features...\n" }
  my @features;
  for (my $y = 0; $y < $tnt->{height}; $y ++) {
    for (my $x = 0; $x < $tnt->{width}; $x ++) {
      my $anim = $tnt->{anims}[ $tnt->{mapFeature}[$y][$x] ];
      if (defined $anim) {
        push @features, { y => $y, x => $x, anim => $anim };
      }
    }
  }

  # Next collect OTA mission features into the map.
  # First we need to select the schema to use.
  if (DEBUG) { print STDERR "Examining ota features...\n" }
  my $schema_number = int(rand($ota->{globalheader}{schemacount}));
  $schema_number = 0;

  # get all features from the ota file
  for (sort keys %{$ota->{globalheader}{'schema ' . $schema_number}{features}}) {
    my $val = $ota->{globalheader}{'schema ' . $schema_number}{features}{$_};
    push @features, { y => $val->{zpos}, x => $val->{xpos}, anim => lc($val->{featurename}) };
  }

  # Feature removal ("footprints").
  # Order of testing is TNT top to bottom, left to right, then any from
  #  the .ota in order (they are numbered).
  # Indestructible features can be placed only if their footprint does not
  #  overlap another.
  # Destructible features follow the same rule, but also: placing one will
  #  overwrite any other destrructible feature in the same spot.
  # Indestructible features will erase destructible.
  # Due to engine reasons, this situation:
  # D D   I I
  # D X X X I
  #   X X X
  # will cause X to overwrite destructible D, then also fail to place because
  #  indestructible I exists, and so only I will render.

  if (DEBUG) { print STDERR "Verifying features...\n" }
  # first pass
  my @footprints;
FEATURE:
  for my $i (0 .. $#features) {
    my $feature = $features[$i];

    # sanity test
    if (! exists $tdf{$feature->{anim}}) {
      die "Error: feature $i ($feature->{anim}) not found in tdf files.";
    }

    # delete feature if footprint extends off map
    if ($tdf{$feature->{anim}}{footprintz} + $feature->{y} > $tnt->{height} ||
        $tdf{$feature->{anim}}{footprintx} + $feature->{x} > $tnt->{width}) {
      if (DEBUG) { print STDERR " !!! Warning: Feature $i ($feature->{anim}) at $feature->{y}, $feature->{x} is out of bounds\n" }
      $features[$i] = undef;
      next FEATURE;
    }

    # test if it can be placed OK
    for my $y ($feature->{y} .. $feature->{y} + $tdf{$feature->{anim}}{footprintz} - 1) {
      for my $x ($feature->{x} .. $feature->{x} + $tdf{$feature->{anim}}{footprintx} - 1) {
        # get any ID from this place
        my $old_id = $footprints[$y][$x];
        if (defined $old_id && defined $features[$old_id]) {
          # ID existed and so did the feature...
          if ($tdf{$features[$old_id]{anim}}{indestructible}) {
            # and it was indestructible, darn.  Erase self and go to the next.
            if (DEBUG) { print STDERR " !!! Warning: Failed to place feature $i ($feature->{anim}) at $feature->{y}, $feature->{x}\n    (Overlaps with $old_id ($features[$old_id]{anim}) at $features[$old_id]{y}, $features[$old_id]{x}\n" }
            $features[$i] = undef;
            next FEATURE;
          } else {
            # it was estructible.  Erase it, instead.
            if (DEBUG) { print STDERR " !!! Warning: Overwriting prev. with new feature $i ($feature->{anim}) at $feature->{y}, $feature->{x}\n    (Overlaps with $old_id ($features[$old_id]{anim}) at $features[$old_id]{y}, $features[$old_id]{x}\n" }
            $features[$old_id] = undef;
          }
        }
        # passed the gauntlet, now write to the footprints array
        $footprints[$y][$x] = $i;
      }
    }
  }

  # reclaim the memory from the footprints table
  undef @footprints;

  # remove holes in the features array
  @features = grep { defined $_ } @features;

  # read GAF of each used feature
  if (DEBUG) { print STDERR "Reading gaf files...\n" }
  my %gafs;
  my %gafs_shad;
  # we need a unique list of gafs to open
  foreach my $anim (uniqstr (map { $_->{anim} } @features)) {
    # if this feature's not loaded yet, load it
    if (! $gafs{$anim}) {
      # locate feature in tdf list
      if (DEBUG) { print STDERR " . $anim ($tdf{$anim}{filename})\n" }
      if ($tdf{$anim}{filename}) {
        my $gaf_path = 'totala/anims/' . $tdf{$anim}{filename} . '.GAF';
        $gafs{$anim} = _gaf(ci_path($gaf_path), $tdf{$anim}{seqname});
        if ($tdf{$anim}{seqnameshad} && ! $gafs_shad{$anim}) {$gafs_shad{$anim} = _gaf(ci_path($gaf_path), $tdf{$anim}{seqnameshad}); }
      }
    }
  }

  # create output image
  if (DEBUG) { print STDERR "Rendering tile map...\n" }
  my @image;
  for (my $y = 0; $y < ($tnt->{height} / 2) - 4; $y ++) {
    if (DEBUG) { print STDERR " $y" }
    for (my $x = 0; $x < ($tnt->{width} / 2) - 1; $x ++) {
      # plot tile
      my $idx = $y * $tnt->{width} / 2 + $x;
      my $t = 32 * $y;
      my $s = 32 * $x;
      for my $i (0 .. 31) {
        push @{$image[$t + $i]}, @{$tnt->{tileGfx}[$tnt->{mapData}[$idx]][$i]};
      }
    }
  }
  if (DEBUG) { print STDERR "\n" }

  # blit all features, sorted left-to-right, back-to-front.
  if (DEBUG) { print STDERR "Blit features...\n" }
  foreach my $feature (sort { $a->{y} <=> $b->{y} or $a->{x} <=> $b->{x} } @features) {
    #print "Plot feature $f_idx (" . $tnt->{anims}[$f_idx] . ") at $y, $x\n";
    my ($x, $y) = ($feature->{x}, $feature->{y});
    if (DEBUG) { print STDERR " . $y, $x: $feature->{anim} " }

    # features use the height of the center of the cell, which is the
    #  average of heights of the four corners
    # computed height then offsets Y value upwards by height/2 pixels

    my $y2 = min($y+1, $tnt->{height} - 1);
    my $x2 = min($x+1, $tnt->{width} - 1);
    my $height = int((
      $tnt->{mapHeight}[$y][$x] +
      $tnt->{mapHeight}[$y2][$x] +
      $tnt->{mapHeight}[$y][$x2] +
      $tnt->{mapHeight}[$y2][$x2]
    ) / 4);

    # objects are also offset by feature footprint * 8px (effectively "centered" around the point)
    my $foot_x = $tdf{$feature->{anim}}{footprintx} * 8;
    my $foot_y = $tdf{$feature->{anim}}{footprintz} * 8;

    my $gaf_shad = $gafs_shad{$feature->{anim}};
    if ($gaf_shad) {
      if (DEBUG) { print STDERR "(shadow) " }
      # shadow is a translucet blit:
      #  find the new RGB values as (pixel1 + pixel2) / 2, then
      #  locate the nearest palette entry that matches and use that.
      my $t = (16 * $y) - $gaf_shad->{yPos} - int($height / 2) + $foot_y;
      my $s = (16 * $x) - $gaf_shad->{xPos} + $foot_x;
      for (my $j = 0; $j < @{$gaf_shad->{data}}; $j ++) {
        next if ($j + $t < 0); last if ($j + $t >= @image);
        for (my $i = 0; $i < @{$gaf_shad->{data}[$j]}; $i ++) {
          next if ($i + $s < 0); last if ($i + $s >= @{$image[$j + $t]});
          my $pixel = $gaf_shad->{data}[$j][$i];
          if (defined $pixel && $pixel != $gaf_shad->{transparentIndex}) {
            # compute RGB blend of shadow + existing pxl
            $image[$t+$j][$s+$i] = $alp->[$image[$t+$j][$s+$i]][$pixel];
          }
        }
      }
    }

    my $gaf = $gafs{$feature->{anim}};
    if ($gaf) {
      if (DEBUG) { print STDERR "(main)" }
      my $t = (16 * $y) - $gaf->{yPos} - int($height / 2) + $foot_y;
      my $s = (16 * $x) - $gaf->{xPos} + $foot_x;
      for (my $j = 0; $j < @{$gaf->{data}}; $j ++) {
        next if ($j + $t < 0); last if ($j + $t >= @image);
        for (my $i = 0; $i < @{$gaf->{data}[$j]}; $i ++) {
          next if ($i + $s < 0); last if ($i + $s >= @{$image[$j + $t]});
          my $pixel = $gaf->{data}[$j][$i];
          if (defined $pixel && $pixel != $gaf->{transparentIndex}) {
            $image[$t+$j][$s+$i] = $pixel;
          }
        }
      }
    }
    if (DEBUG) { print STDERR "\n" }
  }

  if (DEBUG) { print STDERR "Writing bitmap...\n" }
  my $bmp = Common::save_bmp($pal, \@image);
  if (DEBUG) { print STDERR "Calling optipng...\n" }
  my $png = Common::optipng( $bmp );

  if (DEBUG) { print STDERR "Successfully rendered map: '$png'\n" }

  # compose some info about the map
  $filename =~ s/^maps\/totala\///;
  my $desc = sprintf("TOTAL ANNIHILATION\nFilename: %s\nName: %s\nDescription: %s\nDimensions: %s\nPlayers: %s", $filename, $ota->{globalheader}{missionname}, $ota->{globalheader}{missiondescription}, $ota->{globalheader}{size}, $ota->{globalheader}{numplayers});

  return {
    type => 'file',
    data => $png,
    text => $desc,
    extra => ''
  };
}

1;
