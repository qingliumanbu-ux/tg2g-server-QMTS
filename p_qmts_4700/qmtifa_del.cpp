/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 任龙
日期: 2012-03-16
功能: 氧化铁粉实绩信息删除
修改历史：
	日期:________；修改人：________; 需求提出人________
	变更内容:

*************************************************/
/*<remark>=========================================================
/// <summary>
/// 删除氧化铁粉实绩信息
/// <para>
/// 1. 读取前台传入参数；
/// 2. 获取表列名；
/// 3. 建立新增历史表语句；
/// 4. 建立删除语句；
/// 5. 执行删除操作，返回删除结果。
/// </para>
/// </summary>
/// <param name="TABLE_NAME">界面名称</param>
/// <param name="sql_del">拼接sql语句</param>
/// <returns>删除基表信息</returns>
===========================================================</remark>*/

#include "stdafx.h"


BM2F_ENTERACE(qmtifa_del);


int f_qmtifa_del(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);	

	int  doFlag = 0;
	
	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";

	/*获取系统当前时间*/
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	int logFlag = 1;
	// 建立连接
	CDbCommand cmd(conn);

	try
	{
		CString lot_no_pe = (CString)bcls_rec->Tables[1].Rows[0]["LOT_NO_PE"].ToString();

		//声明删除语句
		sqlstr = "DELETE FROM TQMTIFA WHERE LOT_NO_PE =@lot_no_pe ";
		
		/*连接数据库，对在线表进行操作*/
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("lot_no_pe",lot_no_pe);
		cmd.ExecuteNonQuery();

		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);
	}

	/*捕获数据库操作异常*/
	catch(CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);	
		s.flag = -1;
		/*数据库异常时返回-1，事务将被回滚*/
		doFlag = -1;   
	}
	/*捕获应用错误*/
	catch(CApplicationException& ex)
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

	s.flag = doFlag;

	return(doFlag);
}