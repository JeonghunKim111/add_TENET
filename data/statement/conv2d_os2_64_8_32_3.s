2 1
{S[k,c,ox,oy,rx,ry]:0<=k<64 and 0<=c<8 and 0<=ox<32 and 0<=oy<32 and 0<=rx<3 and 0<=ry<3}
{S[k,c,ox,oy,rx,ry]->I[c,oy+ry,ox+rx]}
{S[k,c,ox,oy,rx,ry]->W[k,c,ry,rx]}
{S[k,c,ox,oy,rx,ry]->O[k,oy,ox]}
