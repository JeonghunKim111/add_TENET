{S[k,c,ox,oy,rx,ry]->PE[(c+8*(rx+64*ry))%64,k%64]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+8*(rx+64*ry))/64),floor(k/64),oy,ox]}
