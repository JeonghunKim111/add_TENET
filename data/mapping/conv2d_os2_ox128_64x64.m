{S[k,c,ox,oy,rx,ry]->PE[(ox+128*oy)%64,k%64]}
{S[k,c,ox,oy,rx,ry]->T[floor((ox+128*oy)/64),floor(k/64),floor((ox+128*oy)/64)+floor(k/64),c]}