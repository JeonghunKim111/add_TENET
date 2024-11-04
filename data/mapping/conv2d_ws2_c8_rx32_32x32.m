{S[k,c,ox,oy,rx,ry]->PE[(c+8*(rx+32*ry))%32,k%32]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+8*(rx+32*ry))/32),floor(k/32),oy,ox]}
