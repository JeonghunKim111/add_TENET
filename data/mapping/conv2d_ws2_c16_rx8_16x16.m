{S[k,c,ox,oy,rx,ry]->PE[(c+16*(rx+8*ry))%16,k%16]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+16*(rx+8*ry))/16),floor(k/16),oy,ox]}
