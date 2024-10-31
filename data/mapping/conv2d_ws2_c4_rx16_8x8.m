{S[k,c,ox,oy,rx,ry]->PE[(c+4*(rx+16*ry))%8,k%8]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+4*(rx+16*ry))/8),floor(k/8),oy,ox]}
