/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-11
Description: 转炉测厚标准信息新增
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 新增附件管理表
/// <para>
/// 获取输入参数：TQMTS04B(转炉测厚标准信息表) ；
/// </para>
/// <para>数据库表：TQMTS04B(转炉测厚标准信息表));
///
/// 前台TQMTS04B画面的F3(新增)调用    </para>
/// </summary>
/// <param name="TQMTS04B">转炉测厚标准信息表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts04b_ins)

int f_qmts04b_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts04b("TQMTS04B");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	EIClass bcls_rec_snd;
	EIClass bcls_ret_snd;

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts04b.Reset();
			tqmts04b.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			/******************** 赋初值 *************************/
			tqmts04b["REC_CREATOR"] = s.userid;
			tqmts04b["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts04b["REC_REVISOR"] = " ";
			tqmts04b["REC_REVISE_TIME"] = " ";
			tqmts04b.TrimOrBlank();
			tqmts04b.Print();
			tqmts04b.Insert();

		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
