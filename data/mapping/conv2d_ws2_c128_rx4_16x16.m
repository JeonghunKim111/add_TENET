{S[k,c,ox,oy,rx,ry]->PE[(c+128*(rx+4*ry))%16,k%16]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+128*(rx+4*ry))/16),floor(k/16),oy,ox]}