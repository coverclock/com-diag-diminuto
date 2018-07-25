# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
BEGIN { first = 1; }
($1 == "STEP") { if (first == 1) { first = 0; } else { next; } }
($2 == "interrupted") { next; }
($2 == "terminated") { next; }
{ OFS=","; print $1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12; }
