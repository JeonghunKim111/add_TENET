{S[k,c,ox,oy,rx,ry]->PE[(ox+32*oy)%32,k%32]}
{S[k,c,ox,oy,rx,ry]->T[floor((ox+32*oy)/32),floor(k/32),floor((ox+32*oy)/32)+floor(k/32),c]}