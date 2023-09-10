#ifndef __WU_H__
#define __WU_H__


struct box
{
    s32		r0;         /* min value, exclusive */
    s32		r1;         /* max value, inclusive */
    s32		g0;
    s32		g1;
    s32		b0;
    s32		b1;
    s32		vol;
};

#define BOX   33


void Hist3d(s32 *vwt,s32 *vmr,s32 *vmg,s32 *vmb, float *m_2);
void Momt3d(s32 *vwt, s32 *vmr, s32 *vmg, s32 *vmb, float *m_2);
s32 Vol(struct box * cube, s32 mmt[BOX][BOX][BOX]);
s32 Bottom(struct box * cube, u8 dir, s32 mmt[BOX][BOX][BOX]);
s32 Top(struct box * cube, u8 dir, s32 pos, s32 mmt[BOX][BOX][BOX]);
float Var(struct box * cube);
float Maximize(struct box *cube, u8 dir, s32 first, s32 last, s32 *cut, s32 whole_r, s32 whole_g, s32 whole_b, s32 whole_w);
s32 Cut(struct box * set1, struct box * set2);
void Mark(struct box *cube, s32 label, u8 *tag);
s32 wuReduce(u8 *RGBpic, s32 numcolors, s32 picsize);

#endif
