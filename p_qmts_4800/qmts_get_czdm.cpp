/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2011 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************	
*  程序名称			: qmts_get_czdm
*  程序描述			: 根据不同情况,取得对应的材料处置代码QMCY
*  备注说明			: 
*  修改历史			: 			
*  		2011-08-11 	陈文琼			(ADD)程序建立
*			... ...
* **************************************************************************** */
//此程序目前无用！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


// service入口
BM2F_ENTERACE(qmts_get_czdm)


int f_qmts_get_czdm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	//APP_BEGIN()
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int fetchRowCount;
	
	/* 实体类定义 */
	CModel tep0002("TEP0002");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq1(conn);

	try
	{
		/* ***** 获取输入参数 ***** */
		tep0002.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		Log::Trace("", "","tep0002.code_class={0}",(const char*)tep0002["CODE_CLASS"].ToString());

		bcls_ret->Tables[0].Columns.Add(tep0002);	

		/* ***** 程序处理 ***** */
		/*EXEC SQL DECLARE tep0002_q CURSOR FOR 
		SELECT *
			FROM TEP0002
		 WHERE CODE_CLASS = :tep0002.code_class
  			 AND CODE_DESC_2_CONTENT = :tep0002.code_desc_2_content
		 ORDER BY code ASC; */
		//定义数据库操作命令对象comm_inq执行sql语句，sql字符串用""包括，可以分行，但每行前后务必留出一个空格。
		sqlstr = " SELECT * "
				 " FROM TEP0002 "
				 " WHERE code_class = @code_class "
				 " AND code_desc_2_content = @code_desc_2_content "
				 " ORDER BY code ASC ";
		cmd_inq1.SetCommandText(sqlstr);
		//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
		cmd_inq1.Parameters.Set("code_class", tep0002["CODE_CLASS"].ToString());
		cmd_inq1.Parameters.Set("code_desc_2_content", tep0002["CODE_DESC_2_CONTENT"].ToString());
		cmd_inq1.ExecuteReader();
		while (cmd_inq1.Read())
		{
			cmd_inq1.Fetch(tep0002); 

			CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
			row.Merge(tep0002);   //将实体类的值写入新增行中
			Log::Trace("", "", "prod_code = {0} ,prod_cname = {1} ,prod_ename = {2}", (const char*)tep0002["CODE"].ToString(),(const char*)tep0002["CODE_DESC_1_CONTENT"].ToString(),(const char*)tep0002["CODE_DESC_2_CONTENT"].ToString());
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

	//在函数退出前，统一Close()操作
	cmd_inq1.Close();

	return doFlag;
}
