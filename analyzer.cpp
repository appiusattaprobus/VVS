#include <stdio.h>

int main()
{
    enum state {H, P, S, R, Q, ER}; /* множество состояний */
    enum state CS; /* CS - текущее состояние */
    FILE *fp;/*указатель на файл, в котором находится анализируемая цепочка */
    int c, b;

    CS = Q;
    fp = fopen("data.txt","r");
    c = fgetc(fp);
    do
    {
        switch(CS)
        {
            case Q:
                if(c == '1')
                {
                    CS = P;
                    if(feof(fp)) { CS = ER; break; }
                    c = fgetc(fp);
                }
                else 
                {
                    CS = ER;
                    break;
                }
            break;

            case P:
                if(c == '*')
                {
                    if(feof(fp)){CS = ER; break;}
                    CS = S;
                    c = fgetc(fp);
                }
                else if(c == '$')
                {
                    if(feof(fp)){CS = ER; break;}
                    CS = H;
                    c = fgetc(fp);
                }
                else 
                {
                    CS = ER;
                    break;
                }
            break;
            
            case S:
                if(c == '0')
                {
                    if(feof(fp)){CS = ER; break;}
                    CS = R;
                    c = fgetc(fp);
                }
                else 
                {
                    CS = ER;
                    break;
                }
            break;
            
            case R:
                if(c == '/')
                {
                    if(feof(fp)){CS = ER; break;}
                    CS = Q;
                    c = fgetc(fp);
                }
                else if(c == '$')
                {
                    if(feof(fp)){CS = ER; break;}
                    CS = H;
                    c = fgetc(fp);
                }
                else 
                {
                    CS = ER;
                    break;
                }
            break;
        }
    }while(CS != H && CS != ER);

    if(CS == H)
        printf("This line belong to language! We are happy. =)\n");
    else if(CS = ER)
        printf("Sorry, but this line not belong to language! We are sadness. =(\n");

    if(CS == ER)
        return 0;
    else
        return 1;
}

