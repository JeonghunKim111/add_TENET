{S[k,c,ox,oy,rx,ry]->PE[(c+128*(rx+4*ry))%8,k%8]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+128*(rx+4*ry))/8),floor(k/8),oy,ox]}
