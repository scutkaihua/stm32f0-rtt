
#include "cbuffer.h"
#include <stdlib.h>
#include "string.h"

/*�� cbuf ���� maxlen �ֽڴ�С*/
BOOL cbuffer_init(CBuffer* cbuf,unsigned int maxlen)
{
    if(cbuf->pbuf!=NULL)
    {
        if(cbuf->maxlen==maxlen)
            return TRUE;
        free(cbuf->pbuf);
        cbuf->maxlen=0;
        cbuf->rptr=0;
        cbuf->wptr=0;
    }
    cbuf->pbuf = malloc(maxlen);
    if(cbuf->pbuf==NULL)return FALSE;
    cbuf->maxlen=maxlen;
    cbuf->rptr=0;
    cbuf->wptr=0;
    return TRUE;
}

/*�ͷ�cbuf*/
BOOL cbuffer_free(CBuffer* cbuf)
{
    if(cbuf->pbuf!=NULL)
    {
        free(cbuf->pbuf);
        cbuf->maxlen=0;
        cbuf->rptr=0;
        cbuf->wptr=0;
    }
    return TRUE;
}

/*��ȡcbuf һ���ֽڷ��� readdata*/
BOOL cbuffer_read(CBuffer* cbuf,char* readdata)
{
    unsigned int temp;
    if(cbuf->pbuf==NULL||cbuf->rptr>=cbuf->maxlen)
        return FALSE;
    //����Ϊ��
    if(cbuf->rptr==cbuf->wptr)
        return FALSE;
    else
    {
        //������������,������
        if(readdata)
            *readdata=cbuf->pbuf[cbuf->rptr];
        //������ָ��,�Է�ֹ��ָ��Խ��
        temp= cbuf->rptr;
        temp= (temp+1)%cbuf->maxlen;
        cbuf->rptr=temp;
        return TRUE;
    }
}


/*дһ���ֽ�writedata ��cbuf*/
BOOL cbuffer_write(CBuffer* cbuf,char writedata)
{
    unsigned int temp;
    if(cbuf->pbuf==NULL||cbuf->wptr>=cbuf->maxlen)
        return FALSE;
    //����Ϊ��
    if(cbuf->rptr==((cbuf->wptr+1)%cbuf->maxlen))
        return FALSE ;
    else
    {
        //�����п�λ��,д����
        cbuf->pbuf[cbuf->wptr]=writedata;
        //����дָ��,�Է�ֹдָ��Խ��
        temp= cbuf->wptr;
        temp= (temp+ 1)%cbuf->maxlen;
        //gt_def_dint();
        cbuf->wptr=temp;
        //gt_def_enit();
        return TRUE;
    }
}

/*���cbuf�Ƿ���*/
BOOL cbuffer_isfull(CBuffer* cbuf)
{
    if(cbuf->pbuf==NULL||cbuf->wptr>=cbuf->maxlen)
        return TRUE;
    if(cbuf->rptr==((cbuf->wptr+1)%cbuf->maxlen))
        return TRUE;
    return FALSE;
}

/*���cbuf����*/
BOOL cbuffer_clear(CBuffer* cbuf)
{
    if(cbuf->pbuf==NULL)
        return FALSE;
    //gt_def_dint();
    cbuf->wptr = cbuf->rptr=0;
		memset(cbuf->pbuf,0,cbuf->maxlen);
    //gt_def_enit();
    return TRUE;
}


/*��block
* size:Ҫ��ȡ�Ĵ�С
* return :��ȡ���Ĵ�С
*/
int cbuffer_readbk(CBuffer* cbuf,char* dst,int size)
{
    unsigned int temp1,temp2;
    char* pdat = dst;
    if(    (cbuf->pbuf==NULL) 
        || (cbuf->rptr>=cbuf->maxlen))
        return 0;
    //����Ϊ��
    if(cbuf->rptr==cbuf->wptr)
        return 0;    
    else
    {
        temp1 = cbuf->rptr;
        temp2 = cbuf->wptr;
        while( (temp2!=temp1) && size >0 )
        {
            *pdat++ = cbuf->pbuf[temp1];
            temp1= (temp1+1)%cbuf->maxlen;
            size--;
        }
        cbuf->rptr=temp1;
        return (int)(pdat-dst);
    }
}

//����д����ֽ���
int cbuffer_writebk(CBuffer* cbuf,const char* src,int size)
{
    unsigned int temp1,temp2;
    const char* pdat=src;
    if(cbuf->pbuf==NULL||cbuf->wptr>=cbuf->maxlen)
        return 0;
    temp1 = cbuf->rptr;
    temp2 = cbuf->wptr;   
    while(size >0)
    {
        //����
        if(temp1 == ((temp2 +1)%cbuf->maxlen))break;
        cbuf->pbuf[temp2]=*pdat++;
        temp2 = (temp2+1)%cbuf->maxlen;
        size -- ;
    }
    cbuf->wptr = temp2;
    return (int)(pdat-src);
}

