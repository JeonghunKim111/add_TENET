2 1
{S[k,c,ox,oy,rx,ry]:0<=k<8 and 0<=c<8 and 0<=ox<64 and 0<=oy<64 and 0<=rx<64 and 0<=ry<64}
{S[k,c,ox,oy,rx,ry]->I[c,oy+ry,ox+rx]}
{S[k,c,ox,oy,rx,ry]->W[k,c,ry,rx]}
{S[k,c,ox,oy,rx,ry]->O[k,oy,ox]}
