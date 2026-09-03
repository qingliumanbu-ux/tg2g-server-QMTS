/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   尧玉梅
Version:    1.0
Date:     2017-06-06
Description: 内部钢种人工综判履历查询
**************************************************/

#include "stdafx.h"
 

/*<remark>=========================================================
/// <summary>
/// 查询合同处理履历
/// <para>
/// </para>
/// <para>数据库表：tqmts99        </para>
/// <para>主调用函数：前台QMTSLG画面F2调用。   </para>
/// </summary>
/// <param name="order_no_from">合同号起    </param>
/// <param name="order_no_to">合同号止               </param>
/// <returns>查询合同处理履历</returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmtslg_inq)

int f_qmtslg_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	CTracer log(__FUNCTION__);

	//程序用变量
	int doFlag = 0;
	int fetchRowCount = 0;
	int i = 0;

	/* 业务变量 */
	CString   v_keyvalue_1 = "";                          /* 合同号起 */
	
	//修改开始 吴玉玲  06/10/11
	int  v_flag = 0;
	int  v_every_page = 50;
	int  v_now_page = 1;

	int  v_count;
	int  v_total_page = 0;
	int  v_next_page = 1;
	int  v_now_row = 1;
	//修改结束  吴玉玲  06/10/11  添加变量

	/* 实体类定义 */
	CModel tqmts99("TQMTS99");

	CString  sqlstr("");              // 数据库SQL操作字符串
	/* 数据库操作类定义 */
	CDbCommand cmd_tqmtslg_inq(conn);
	CDbCommand cmd_inq(conn);

	try
	{

		v_every_page = bcls_rec->Tables[1].Rows[0]["every_page"];
		v_now_page = bcls_rec->Tables[1].Rows[0]["now_page"];
		Log::Trace("", "", "传入v_now_page[{0}]", v_now_page);

		v_keyvalue_1 = bcls_rec->Tables[0].Rows[0]["keyvalue_1"];
		
		
	
		
		Log::Trace("", "", "v_keyvalue_1		= [{0}]", (const char*)v_keyvalue_1);
		

		//修改开始 吴玉玲  06/10/11

		//获得数据量
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = CString(" SELECT count(1) "
				" FROM tqmts99 "
				" WHERE 1=1");
			break;
		}

		if (v_keyvalue_1.TrimOrBlank() != " ")
			sqlstr = sqlstr + "  AND keyvalue_1 like  '%'||@v_keyvalue_1||'%'";


		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("v_keyvalue_1", v_keyvalue_1);
		
		

		v_count = cmd_inq.ExecuteScalar().ToInt32();
		Log::Trace("", "", "v_count=[{0}]", v_count);

		if (v_count%v_every_page == 0)//得出总页数
		{
			v_total_page = v_count / v_every_page;
		}
		else
		{
			v_total_page = v_count / v_every_page + 1;
		}
		Log::Trace("", "", "return v_count[{0}], v_total_page [{1}] ", v_count, v_total_page);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = CString("SELECT * "
				" FROM tqmts99 "
				" WHERE 1=1");
			break;
		}

		if (v_keyvalue_1.TrimOrBlank() != " ")
			sqlstr = sqlstr + "  AND keyvalue_1 like  '%'||@v_keyvalue_1||'%'";

		
	
		cmd_tqmtslg_inq.SetCommandText(sqlstr);
		cmd_tqmtslg_inq.Parameters.Set("v_keyvalue_1", v_keyvalue_1);
		

		cmd_tqmtslg_inq.ExecuteQuery(bcls_ret->Tables[0], ((v_now_page - 1)*v_every_page), v_every_page);
		Log::Trace("", "", "((v_now_page-1)*v_every_page)[{0}]v_every_page[{1}]", ((v_now_page - 1)*v_every_page), v_every_page);

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "total_page");
		bcls_ret->Tables[1].Columns.Add(DT_STRING, "total_count");
		bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[0]["total_page"] = v_total_page;
		bcls_ret->Tables[1].Rows[0]["total_count"] = v_count;
		Log::Trace("", "", "return v_total_page[{0}] v_count [{1}]", v_total_page, v_count);

		CFormattable arguments[] = { v_count };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000004")/*查询到[{0}]条记录。*/, arguments, 1);


		//修改结束  吴玉玲  06/10/11  添加next_page total_page返回前台程序
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "", "error=[{0}]", (const char*)str);

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
