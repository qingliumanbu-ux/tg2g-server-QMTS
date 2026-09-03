/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2019
Author:      质保书标准信息表
Version:     1.0
Date:        2023-10-26
Description: 0RT803
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

/*<remark>=========================================================
/// <summary>
/// 质保书标准信息表
///
/// </summary>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
#include "epex.h" 


//外部函数声明
int f_tableObjectCheck9999(ITableObject2& obj);	//字段超长检测

BM2F_ENTERACE_TELE(cm_0rt803_rcv)

int f_cm_0rt803_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int blkNum_pmol02 = 0;
	/* 业务变量 */
	CString	datetime("");
	/* 业务变量 */

	/* 实体类定义 */
	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_test(conn);
	CModel tqmts0r03("TQMTS0R03");
	CString heat_no = " ";
	CString dev_code = "";
	CString sm_plan_no = " ";
	CString cname = " ";

	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		/*tqmts0r03.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		f_tableObjectCheck9999(tqmts0r03);
		if (tqmts0r03.QueryCount("CERTI_PRINT_NO,CERTI_BILL_NO") > 0){
			tqmts0r03.Delete("CERTI_PRINT_NO,CERTI_BILL_NO");
			tqmts0r03.Insert();
		}
		else{
			tqmts0r03.Insert();
		}*/
		//sprintf(s.msg, "%d条记录新增成功！请重新查询！");
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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
	//返回-1时事务将回滚，返回为0是事务将提交	return doFlag;
}


