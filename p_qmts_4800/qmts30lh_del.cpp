/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-04-18
Description: 成分不合处置关闭
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件

//外部函数声明

BM2F_ENTERACE(qmts30lh_del)

int f_qmts30lh_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");



	/* 业务变量 */
	CModel tqmts30("TQMTS30");
	CModel hqmts30("HQMTS30");
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts30["HEAT_NO"] = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString().TrimOrBlank();
			tqmts30["AREA"] = bcls_rec->Tables[0].Rows[i]["AREA"].ToString().TrimOrBlank();

			if (tqmts30.Query())
			{
				hqmts30.CopyFrom(tqmts30);
				hqmts30["REC_CREATOR"] = s.userid;
				hqmts30["REC_CREATE_TIME"] = datetime;
				hqmts30.Insert();
			}
			tqmts30.Delete("HEAT_NO,AREA");
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


