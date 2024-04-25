#!/usr/bin/perl

use strict;
use warnings qw(all FATAL);
$| = 1;

my $CUTOFF = 0.05;
my @knock = map { $_ - $ARGV[0] } @ARGV;

my @window;
while (<STDIN>) {
	chomp;
	if ($_ ne "--") {
		push @window, $_;
		next;
	}

	my $score = score(@window);
	@window = ();
	if ($score < $CUTOFF) {
		print "ok\n";
	} else {
		print "bad\n";
	}
}

sub score {
	my @w = map { $_ - $_[0] } @_;

	if (@w != @knock) {
		print STDERR "wrong number of knocks\n";
		return $CUTOFF + 1;
	}

	my $ret = 0;
	for my $i (0..$#knock) {
		my $v = $knock[$i] - $w[$i];
		$ret += $v * $v;
	}
	print STDERR "score: $ret\n";
	return $ret;
}
