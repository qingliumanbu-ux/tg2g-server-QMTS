/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-11
Description: 查询附件信息
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 删除附件管理表
/// <para>
/// 获取输入参数：TQMTS10(制造标准-附件管理表) ；
/// </para>
/// <para>数据库表：TQMTS10(制造标准-附件管理表));
///
/// 前台QMTS10画面的F5(删除)调用    </para>
/// </summary>
/// <param name="TQMTS10">附件管理表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts10_del)

int f_qmts10_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts10("TQMTS10");
	EIClass bcls_rec_snd;
	EIClass bcls_ret_snd;
	/* 数据库操作类定义：统一放在Service或函数前段 */

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts10.Reset();
			tqmts10.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (tqmts10["SEQ_NO"].ToDecimal() == 0)
			{
				sprintf(s.msg, "附件管理表不可为空");/*附件管理表不可为空。*/
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tqmts10.Delete("SEQ_NO"); //条件字段项
		}

		sprintf(s.msg,"删除完毕");/*删除完毕。*/
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
