{S[k,c,ox,oy,rx,ry]->PE[(c+16*(rx+3*ry))%32,k%32]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+16*(rx+3*ry))/32),floor(k/32),oy,ox]}
