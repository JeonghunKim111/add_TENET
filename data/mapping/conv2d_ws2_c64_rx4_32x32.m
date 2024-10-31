{S[k,c,ox,oy,rx,ry]->PE[(c+64*(rx+4*ry))%32,k%32]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+64*(rx+4*ry))/32),floor(k/32),oy,ox]}
