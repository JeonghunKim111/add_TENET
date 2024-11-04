{S[k,c,ox,oy,rx,ry]->PE[(c+8*(rx+3*ry))%128,k%128]}
{S[k,c,ox,oy,rx,ry]->T[floor((c+8*(rx+3*ry))/128),floor(k/128),oy,ox]}
