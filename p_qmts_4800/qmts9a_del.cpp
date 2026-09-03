/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-09
Description: 渣样实绩后备删除
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///渣样实绩后备删除
/// <para>
/// 1.渣样实绩后备删除
///
/// </para>
/// <para>数据库表：TQMTS26(实绩_渣样成分)				</para>
/// <para>主调用函数：前台QMTS26画面的F5(删除)调用		</para>
/// <para>需调用函数：									/para>
/// </summary>
/// <param name="heat_no">熔炼号				</param>
/// <param name="st_sample_no">试样号			</param>
/// <returns>  </returns>
===========================================================</remark>*/


// service入口
BM2F_ENTERACE(qmts26_del)


int f_qmts26_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts26("TQMTS26");
	CModel tqmts24("TQMTS24");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/* 取得单行传入信息 */
			tqmts24.Reset();
			tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "", "qmts26_del IN:---heat_no[{0}]", (const char*)tqmts24["HEAT_NO"].ToString());
			Log::Trace("", "", "qmts26_del IN:---st_sample_no[{0}]", (const char*)tqmts24["ST_SAMPLE_NO"].ToString());

			//校验传入参数
			if (tqmts24["HEAT_NO"].ToString().Trim() == "" || tqmts24["HEAT_NO"].ToString().Trim() == " ")
			{
				strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tqmts24["ST_SAMPLE_NO"].ToString().Trim() == "" || tqmts24["ST_SAMPLE_NO"].ToString().Trim() == " ")
			{
				strcpy(s.msg, _RES("QM00S0004260")/*请选择或输入试样号。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts26["HEAT_NO"] = tqmts24["HEAT_NO"];
			tqmts26["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			tqmts26.Delete("HEAT_NO,ST_SAMPLE_NO"); //条件字段项
			tqmts24.Delete("HEAT_NO,ST_SAMPLE_NO"); //条件字段项
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
