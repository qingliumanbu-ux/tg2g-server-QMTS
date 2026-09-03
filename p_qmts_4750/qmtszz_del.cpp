/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-28
Description: 删除组合元素公式代码
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 删除组合元素公式代码
/// <para>
/// 1.删除组合元素公式代码
/// </para>
/// <para>数据库表：tep0002(代码值集信息表)                </para>
/// <para>主调用函数：前台QMTSZZ画面的F5(删除)调用。       </para>
/// </summary>
/// <param name="CODE_CLASS"> 代码编号                     </param>
/// <param name="CODE"> 代码                               </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */

// service入口
BM2F_ENTERACE(qmtszz_del)

int f_qmtszz_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/*获得传入参数*/
		tep0002.Reset();
		tep0002.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Trace("", "","qmtszz_del IN:---CODE_CLASS = [{0}]",(const char*)tep0002["CODE_CLASS"].ToString());
		Log::Trace("", "","qmtszz_del IN:---CODE = [{0}]",(const char*)tep0002["CODE"].ToString());

		//删除工艺卡表
		tep0002.Delete("CODE_CLASS,CODE");
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
