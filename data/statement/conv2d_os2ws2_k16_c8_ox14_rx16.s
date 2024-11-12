2 1
{S[k,c,ox,oy,rx,ry]:0<=k<16 and 0<=c<8 and 0<=ox<14 and 0<=oy<14 and 0<=rx<16 and 0<=ry<16}
{S[k,c,ox,oy,rx,ry]->I[c,oy+ry,ox+rx]}
{S[k,c,ox,oy,rx,ry]->W[k,c,ry,rx]}
{S[k,c,ox,oy,rx,ry]->O[k,oy,ox]}
