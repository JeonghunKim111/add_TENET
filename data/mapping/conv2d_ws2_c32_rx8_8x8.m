{S[k,c,ox,oy,rx,ry]->PE[(c+32*(rx+8*ry))%8,k%8]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+32*(rx+8*ry))/8),floor(k/8),oy,ox]}
