{S[k,c,ox,oy,rx,ry]->PE[(ox+32*oy)%8,k%8]}
{S[k,c,ox,oy,rx,ry]->T[floor((ox+32*oy)/16),floor(k/16),floor((ox+32*oy)/16)+floor(k/16),c]}
