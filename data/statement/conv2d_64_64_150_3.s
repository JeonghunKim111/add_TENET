2 1
{S[k,c,ox,oy,rx,ry]:0<=k<64 and 0<=c<64 and 0<=ox<150 and 0<=oy<150 and 0<=rx<3 and 0<=ry<3}
{S[k,c,ox,oy,rx,ry]->I[c,ox+rx,oy+ry]}
{S[k,c,ox,oy,rx,ry]->W[k,c,rx,ry]}
{S[k,c,ox,oy,rx,ry]->O[k,ox,oy]}
