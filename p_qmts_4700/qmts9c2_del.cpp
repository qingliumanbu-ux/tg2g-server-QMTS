/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      李翔宇
Version:     1.0
Date:        2023-05-04
Description: 删除保护渣基准管理
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 删除混杂元素管理
/// <para>
/// 获取输入参数：TQMTS9C2(混杂元素管理表) ；
/// </para>
/// <para>数据库表：TQMTS9C2(混杂元素管理表));
///
/// 前台QMTS9C2画面的F5(删除)调用    </para>
/// </summary>
/// <param name="TQMTS9C2">混杂元素管理表    </param>
===========================================================</remark>*/


BM2F_ENTERACE(qmts9c2_del)
BM2_FUNCTION_IMPORT


int f_qmts9c2_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts9c2("TQMTS9C2");
	EIClass bcls_rec_snd;
	EIClass bcls_ret_snd;
	/* 数据库操作类定义：统一放在Service或函数前段 */

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts9c2.Reset();
			tqmts9c2.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "", "qmts9c2_del IN:---PATTERN_NO,STEEL_GROUP = [{0},{1}]", (const char*)tqmts9c2["PATTERN_NO"].ToString(), (const char*)tqmts9c2["STEEL_GROUP"].ToString());

			if (tqmts9c2["PATTERN_NO"].ToString().Trim() == "" || tqmts9c2["STEEL_GROUP"].ToString().Trim() == "")
			{
				sprintf(s.msg, _RES("QM00S0004169")/*混杂元素管理区分不可为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tqmts9c2.Delete("PATTERN_NO,STEEL_GROUP"); //条件字段项
#ifdef _SYS_MMS   /*是MMS系统时，发送电文到PES*/
			Log::Trace("", "", "调用函数f_cm_002002_snd");
			tqmts9c2.MergeTo(bcls_rec_snd.Tables[0], false);
			if (!bcls_rec_snd.Tables[0].Columns.Contains("FLAG"))
			{
				bcls_rec_snd.Tables[0].Columns.Add(DT_STRING, "FLAG");
			}
			if (bcls_rec_snd.Tables[0].Rows.get_Count() == 0)
			{
				bcls_rec_snd.Tables[0].Rows.Add();
			}
			bcls_rec_snd.Tables[0].Rows[0]["FLAG"] = "3";
			doFlag = f_cm_002002_snd(&bcls_rec_snd, &bcls_ret_snd, conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif
		}

		sprintf(s.msg, _RES("QM00S0004339")/*删除完毕。*/);
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
