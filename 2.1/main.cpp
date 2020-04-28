//
//  main.cpp
//  2.1
//
//  Created by 何忆源 on 2020/4/21.
//  Copyright © 2020 何忆源. All rights reserved.
//
//首次适应算法实现主存的分配和回收
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const int CANUSE = 1;
const int CANTUSE = 0;
const int MSIZE = 128;//内存分区
struct MZone
{
    int begin_addr;//空闲区起始地址
    int length;//一个连续空闲区的长度
    int state;//状态
    char task_name[32];//内存中任务名
    struct MZone *next;//指向下一个空闲分区
};
struct MZone * Mhead = NULL;//内存头指针
void show()//show 函数，显示当前内存分配情况
{
    struct MZone *Mpoint = Mhead;//Mpoint指向头指针Mhead
    printf(" 内存的使用情况 \n");
    printf("起始地址\t长度\t状态\t任务名\n");
    while( Mpoint!=NULL)//遍历整个链表
    {
        printf("%dk\t\t",Mpoint->begin_addr);
        printf("%dk\t",Mpoint->length);
        Mpoint->state?printf("空闲\t"):printf("占用\t");
        //Mpoint->state为真则输出空闲否则输出已分配
        printf("%s\n",Mpoint->task_name);
        Mpoint = Mpoint->next;//以此向后遍历
    }
}
int Minsert(struct MZone* Mnew)//Minsert 函数，功能插入任务到空闲分区
{
    struct MZone *Zinsert = Mhead;//同上
    while( Zinsert->length<Mnew->length || !Zinsert->state)
    {//空闲分区大小小于要分配的内存大小或者空闲分区完全被占用
        if( Zinsert->next!=NULL )
        {
            Zinsert = Zinsert->next;
        }
        else
        {
            Zinsert = Zinsert->next;
            break;
        }//貌似多余了。。。。
    }
    if( Zinsert==NULL )
    {
        return 0;
    }
    if( MSIZE == Zinsert->begin_addr+Mnew->length )
    {//恰好分配完全
        Zinsert->state = CANTUSE;
        strcpy(Zinsert->task_name , Mnew->task_name);
        //把Mnew->task_name复制给Zinsert->task_name
        Zinsert->next = NULL;
        return 1;
    }
    else
    {//正常情况
        struct MZone *Ztail = (struct MZone*)malloc(sizeof(struct MZone));
        Zinsert->state = CANTUSE;
        strcpy(Zinsert->task_name , Mnew->task_name);
        Zinsert->length = Mnew->length;
        Zinsert->next = Ztail;
        memset( Ztail, 0, sizeof(Ztail) );
        Ztail->begin_addr = Zinsert->begin_addr +Mnew->length;
        Ztail->state = CANUSE;
        Ztail->length = MSIZE - Ztail->begin_addr;
        Ztail->next = NULL;
        return 1;
    }
}
void memoallocate(void)//memoallocate 函数，用于分配内存
{
    struct MZone *Mnew = (struct MZone*)malloc(sizeof(struct MZone));
    printf(" 输入要分配内存大小 (kb):\n");
    scanf("%d",&Mnew->length);
    printf(" 输入任务名 :\n");
    scanf("%s",&Mnew->task_name);
    Minsert(Mnew)?printf(" 分配内存成功 \n"):printf(" 没有符合大小的空闲分区，内存分配失败。 \n");
    show();
    free(Mnew);
}

int Mreturn(char taskname[])//Mreturn 函数，功能回收内存
{
    struct MZone *front = NULL;
    struct MZone *position = Mhead;
    struct MZone *tail = Mhead->next;
    while( strcmp(position->task_name,taskname)!=0 )
    {
        front = position;
        if( position->next!=NULL )
        {
            position = position->next;
        }
        else
        {
            position = NULL;
            break;
        }
        tail = position->next;
    }
    if(position==NULL )
{
    printf(" 内存中没有此任务！ ");
}
    else
    {
        if( NULL!=tail&&NULL!=front )//不能用 CANTUSE
        {
            if( front->state&&tail->state )
            {
                front->length = front->length +position->length + tail->length;
                memset( position->task_name, 0,sizeof(position->task_name));//
                front->next = tail->next;
                position->state = CANUSE;//
                free(tail);
            }
            else if( front->state&&!tail->state )
            {
                front->length = front->length +position->length;
                memset( position->task_name, 0,sizeof(position->task_name));
                front->next = position->next;
                position->state = CANUSE;//
            }
            else if( !front->state&&tail->state )
            {
                position->length = position->length +tail->length;
                memset( position->task_name, 0,sizeof(position->task_name));
                position->next = tail->next;
                position->state = CANUSE;
                free(tail);
            }
            else if( !front->state&&!tail->state )
            {
                memset( position->task_name, 0,sizeof(position->task_name));
                position->state = CANUSE;
            }
        }
        else if( NULL!=tail&&NULL==front )
        {
            if( !tail->state )
            {
                memset( position->task_name, 0,sizeof(position->task_name));
                position->state = CANUSE;
            }
            else
            {
                position->length = position->length +tail->length;
                memset( position->task_name, 0,sizeof(position->task_name));
                position->next = NULL;
                position->state = CANUSE;//
                free(tail);
            }
        }
        else if( NULL==tail&&NULL!=front )
        {
            if(front->state)
            {
                front->length = front->length +position->length;
                memset( position->task_name, 0,sizeof(position->task_name));
                front->next = NULL;
                position->state = CANUSE;
            }
            else
            {
                memset( position->task_name, 0,sizeof(position->task_name));
                position->state = CANUSE;
            }
        }
        else if( tail==NULL&&front==NULL )
        {
            memset( position->task_name, 0,sizeof(position->task_name));
            position->state = CANUSE;
            
        }
        printf(" 内存回收成功！ \n");
        show();
    }
    return 0;
}
void memoreturn(void)//memoreturn 函数，用于回收内存
{
    char tname[32];
    printf(" 输入要收回的任务名 \n");
    scanf("%s",tname);
    Mreturn(tname);
}
int main()
{
    int func_ = 0;
    Mhead = (struct MZone*)malloc(sizeof(struct MZone));//初始化 Mhead
    Mhead->begin_addr = 0;
    Mhead->length = MSIZE;
    Mhead->state = CANUSE;
    memset(Mhead->task_name, 0, sizeof(char)*32 );
    Mhead->next = NULL;
    while( 1 )
    {
        printf(" 首次适应算法实现主存分配和回收系统（内存 MSIZE ）\n");
        printf("|1: 查看内存分配情况 \n");
        printf("|2: 申请分配内存 \n");
        printf("|3: 申请回收内存 \n");
        printf("|4: 退出程序 \n");
        printf("******************************************\n");
        scanf("%d",&func_);
        switch( func_ )
        {
            case 1 :show();break;//输出内存分配情况
            case 2 :memoallocate();break;//申请分配内存
            case 3 :memoreturn();break;//申请回收内存
            case 4 :return 1;
        }
    }
}
