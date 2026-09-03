/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:       
Version:     1.0
Date:        2023-02-6
Description: 查询 
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 查询各个制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)——0:表头内容;1:具体内容)
/// </para>
/// <para>数据库表：TQMTMA8(出钢记号与牌号对照表)
///              
/// 前台各个QMTSA8画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTMA8">出钢记号与牌号对照表    </param>
===========================================================</remark>*/ 

// service入口
BM2F_ENTERACE(qmtsa8_inq)


int f_qmtsa8_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int fetchRowCount;
	int flag = 0;
	int o_flag = 0;
	CString ST_NO("");
	CString REMARK("");
	/* 实体类定义 */
	CModel tqmtma8("TQMTMA8");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		Log::Trace("", "", "qmtsa8_inq 0:");

		CPageInfo pageInfo;
		if (bcls_rec->Tables.Contains("PageInfo"))
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		else
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = -1;
		}
		//初始化
		s.flag = 0;
		s.sqlcode = 0;
		strcpy(s.msg, " ");

		/*获得传入参数*/
		tqmtma8.Reset();

		ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();//出钢记号
		REMARK = bcls_rec->Tables[0].Rows[0]["REMARK"].ToString().Trim();
		Log::Trace("", "", "qmtsa8_inq IN:---ST_NO = [{0}],REMARK = [{1}]", ST_NO, REMARK);
		//if(bcls_rec->Tables[0].Rows.get_Count()>0) tqmtma8.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//Log::Trace("", "", "qmtsa8_inq IN:---st_no = [{0}]", (const char*)tqmtma8["ST_NO"].ToString());

		//查询工艺卡表
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM tqmtma8 "
				"  WHERE 1=1 ";
			if (tqmtma8["ST_NO"].ToString().Trim() != "") sqlstr += " and ST_NO like @ST_NO || '%'";
			if (tqmtma8["REMARK"].ToString().Trim() != "") sqlstr += " and REMARK like @REMARK|| '%'";
			break;
		}

		Log::Trace("", "", "qmtsa8_inq IN:---sqlstr = [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ST_NO", tqmtma8["ST_NO"].ToString().Trim());
		cmd_inq.Parameters.Set("REMARK", tqmtma8["REMARK"].ToString().Trim());
		//执行，获取数据内容到bcls_ret中
		bcls_ret->Tables[0].Clear();
		fetchRowCount = cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		Log::Trace("", "", "qmtsa8_inq out:---fetchRowCount = [{0}]", fetchRowCount);
		cmd_inq.Close();

		//其他返回：表名，记录数
		bcls_ret->Tables[0].set_TableName("TQMTSA8");
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "","error=[{0}]", (const char*)str );

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
