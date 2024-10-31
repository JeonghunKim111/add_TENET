{S[k,c,ox,oy,rx,ry]->PE[(c+64*(rx+8*ry))%16,k%16]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+64*(rx+8*ry))/16),floor(k/16),oy,ox]}
