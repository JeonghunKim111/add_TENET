{S[k,c,ox,oy,rx,ry]->PE[(ox+128*oy)%64,k%128]}
{S[k,c,ox,oy,rx,ry]->T[floor((ox+128*oy)/64),floor(k/128),floor((ox+128*oy)/64)+floor(k/128),c]}
