{S[k,c,ox,oy,rx,ry]->PE[(ox+512*oy)%128,k%128]}
{S[k,c,ox,oy,rx,ry]->T[floor((ox+512*oy)/128),floor(k/128),floor((ox+512*oy)/128)+floor(k/128),c]}
