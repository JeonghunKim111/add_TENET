{S[k,c,ox,oy,rx,ry]->PE[(c+2*(rx+128*ry))%64,k%64]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+2*(rx+128*ry))/64),floor(k/64),oy,ox]}
