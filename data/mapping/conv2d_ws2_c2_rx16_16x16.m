{S[k,c,ox,oy,rx,ry]->PE[(c+2*(rx+16*ry))%16,k%16]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+2*(rx+16*ry))/16),floor(k/16),oy,ox]}