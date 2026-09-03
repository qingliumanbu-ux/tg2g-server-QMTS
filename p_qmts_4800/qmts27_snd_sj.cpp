/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-10
Description: 低倍硫印实绩发送
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/* ***** 外部函数申明 ***** */
//int f_qm200003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);	//低倍硫印实绩上传电文


/*<remark>=========================================================
/// <summary>
///低倍硫印实绩发送
/// <para>
/// 1.低倍硫印实绩发送
/// 
/// </para>
/// <para>数据库表：TQMTS27(实绩_低倍硫印)		</para>
/// <para>主调用函数：前台QMTS27画面的F11(实绩发送)调用				                </para>
/// <para>需调用函数：							</para>
/// </summary>
/// <param name="SLAB_NO">  板坯号				</param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(qmts27_snd_sj)


int f_qmts27_snd_sj(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{	
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	
	CString sqlstr = "";

	//调用低倍硫印实绩上传电文函数
	EIClass bcls_rec_tc;
	EIClass bcls_ret_tc;
	bcls_rec_tc.Tables[0].Columns.Add(DT_STRING,"OP_FLAG");
	bcls_rec_tc.Tables[0].Columns.Add(DT_STRING,"SLAB_NO");
	bcls_rec_tc.Tables[0].Columns.Add(DT_DECIMAL,"ISE_TEST_FLAG");
	bcls_rec_tc.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* ***** 获取输入参数 ***** */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{
			//取得单行传入信息
			tqmts27.Reset();
			tqmts27.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts27_snd_sj IN:---SLAB_NO = [{0}]",(const char*)tqmts27["SLAB_NO"].ToString());
			Log::Trace("", "","qmts27_snd_sj IN:---ISE_TEST_FLAG = [{0}]",tqmts27["ISE_TEST_FLAG"].ToDecimal().ToInt32());

			if (tqmts27["SLAB_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

#ifdef _SYS_PES
			/* ***** 程序处理 ***** */
			Log::Trace("", "","-------发送L4低倍硫印实绩 -----!");
			bcls_rec_tc.Tables[0].Rows[0]["OP_FLAG"] = "1";
			bcls_rec_tc.Tables[0].Rows[0]["SLAB_NO"] = tqmts27["SLAB_NO"];
			bcls_rec_tc.Tables[0].Rows[0]["ISE_TEST_FLAG"] = tqmts27["ISE_TEST_FLAG"];

			//doFlag = f_qm200003_snd(&bcls_rec_tc,&bcls_ret_tc,conn);
			if(doFlag != 0)
			{
				Log::Trace("", "","f_qm200003_snd() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
