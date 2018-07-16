BEGIN { was = 0; }
($1!="luxrheostat:") { next; }
($2!="PWM") { next; }
($3<was) { exit; }
{  print $3","$7","$10","$13","$16; was = $3; }
