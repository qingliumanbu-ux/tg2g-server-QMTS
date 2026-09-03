/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     wsl
Version:    1.0
Date:       2024-08-13
Description:上传图片导入
**************************************************/
//框架头文件
#include "stdafx.h"

//业务头文件
//外部函数声明
BM2F_ENTERACE(qmtssq_pro)

int f_qmtssq_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */
	CModel tqmtssq("TQMTSSQ");
	/* 实体类定义 */
	CDbCommand cmd_inq(conn);

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal cd_count = 0;

	try
	{
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmtssq.Reset();
			tqmtssq.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			int seq_no = 0;
			//查询序列号最大
			CString sqlstr1 = "SELECT MAX(KEY_SEQ) FROM  TQMTSSQ";
			cmd_inq.SetCommandText(sqlstr1);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				seq_no = cmd_inq.GetInt16(1);
			}
			cmd_inq.Close();

			Log::Trace("", "Count", bcls_rec->Tables[0].Rows.get_Count());
			tqmtssq["KEY_SEQ"] = seq_no + 1;
			tqmtssq.TrimOrBlank();
			tqmtssq.Delete("KEY_SEQ");
			tqmtssq.Insert();
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


