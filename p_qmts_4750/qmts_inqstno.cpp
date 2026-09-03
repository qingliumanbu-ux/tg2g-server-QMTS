/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      顾霞
Version:     1.0
Date:        2014-10-30
Description: 查询内部钢种
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 查询各个制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表)
///                 TQMTS01(工序制造标准表)

/// 前台各个QMTS0x画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各制造标准表    </param>
===========================================================</remark>*/ 

int f_qmtsp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//校验工艺卡是否已更新

// service入口
BM2F_ENTERACE(qmts_inqstno)


int f_qmts_inqstno(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int fetchRowCount;

	CString table_name = " ";
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CString v_equ_no = " ";
	CString v_ingot_code = " ";
	CString base_code = "";
	CString s_factory_div = "";  //2023.1.16

	/* 实体类定义 */
	CModel tqmts01("TQMTS01");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	//查询出钢记号
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "STD_FLAG");      //检查标准是否存在
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "VALID_FLAG");    //检查标准是否生效
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");      //update by yiling 20170224  加厂别
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "BASE_CODE");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "EQU_NO");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "INGOT_CODE");
//	bcls_ret->Tables[0].Columns.Add(DT_STRING,"st_no");
//	bcls_ret->Tables[0].Columns.Add(DT_STRING,"std_flag");      //检查标准是否存在
//	bcls_ret->Tables[0].Columns.Add(DT_STRING,"valid_flag");    //检查标准是否生效
//	bcls_ret->Tables[0].Columns.Add(DT_STRING, "factory_div");      //update by yiling 20170224  加厂别
//	bcls_ret->Tables[0].Columns.Add(DT_STRING, "equ_no");
//	bcls_ret->Tables[0].Columns.Add(DT_STRING, "ingot_code");
	try
	{
		/*获得传入参数*/
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV")) s_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		Log::Trace("", "", "qmts_inqstno IN: table_name[{0}]st_no[{1}]whole_backlog_code[{2}]base_code[{3}] FACTORY_DIV[{4}]", (const char*)table_name, (const char*)q_st_no, (const char*)q_whole_backlog_code, (const char*)base_code, (const char*)s_factory_div);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				if (table_name == "TQMTS08" || table_name == "TQMTS0M")
				{
					sqlstr = "SELECT DISTINCT ST_NO,FACTORY_DIV,BASE_CODE,EQU_NO,INGOT_CODE ";
				}
				else
				{
					sqlstr = "SELECT DISTINCT ST_NO,FACTORY_DIV,BASE_CODE ";
				}
				sqlstr = sqlstr +
					"  FROM " + table_name +
					" WHERE ST_NO LIKE @st_no||'%' ";
				sqlstr += "    AND base_code = @base_code ";
				if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = @factory_div";
				sqlstr = sqlstr + " ORDER BY ST_NO ASC ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no",q_st_no.Trim());
		cmd_inq.Parameters.Set("base_code", base_code);
		if (s_factory_div != "") cmd_inq.Parameters.Set("factory_div", s_factory_div);
		cmd_inq.ExecuteReader();

		Log::Trace("", "", "qmts_inqstno sqlstr[{0}]", sqlstr);
		fetchRowCount = 0;
		while(cmd_inq.Read())
		{
			tqmts01["ST_NO"] = cmd_inq.GetString(1);
			tqmts01["FACTORY_DIV"] = cmd_inq.GetString(2);
			tqmts01["BASE_CODE"] = cmd_inq.GetString(3);
			if (table_name == "TQMTS08" || table_name == "TQMTS0M")
			{
				v_equ_no = cmd_inq.GetString(4);
				v_ingot_code = cmd_inq.GetString(5);
			}

			fetchRowCount++;
			Log::Trace("", "", "fetchRowCount[{0}],tqmts01.ST_NO[{1}],tqmts01.FACTORY_DIV[{2}],tqmts01.BASE_CODE[{3}]", fetchRowCount, tqmts01["ST_NO"].ToString(), tqmts01["FACTORY_DIV"].ToString(), tqmts01["BASE_CODE"].ToString());

			bcls_ret->Tables[0].Rows.Add();
			bcls_ret->Tables[0].Rows[fetchRowCount-1]["st_no"] = tqmts01["ST_NO"];
			bcls_ret->Tables[0].Rows[fetchRowCount-1]["base_code"] = tqmts01["BASE_CODE"];
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT IF_PASS"
							"  FROM TQMTS01 "
							" WHERE   "
							"  ST_NO = @st_no "
							"  AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
							"  AND  FACTORY_DIV =@factory_div"
						    "  AND  BASE_CODE =@base_code";
					break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("st_no",tqmts01["ST_NO"].ToString());
			cmd_inq_01.Parameters.Set("whole_backlog_code",q_whole_backlog_code);
			cmd_inq_01.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString().TrimOrBlank());
			cmd_inq_01.Parameters.Set("base_code", tqmts01["BASE_CODE"].ToString().TrimOrBlank());
			cmd_inq_01.ExecuteReader();
			if(cmd_inq_01.Read())
			{
				tqmts01["IF_PASS"] = cmd_inq_01.GetString(1);
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["STD_FLAG"] = "1";
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["VALID_FLAG"] = tqmts01["IF_PASS"];
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["EQU_NO"] = v_equ_no;
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["INGOT_CODE"] = v_ingot_code;
		//		bcls_ret->Tables[0].Rows[fetchRowCount-1]["std_flag"] = "1";
		//		bcls_ret->Tables[0].Rows[fetchRowCount-1]["valid_flag"] = tqmts01["IF_PASS"];
		//		bcls_ret->Tables[0].Rows[fetchRowCount - 1]["factory_div"] = tqmts01["FACTORY_DIV"];
		//		bcls_ret->Tables[0].Rows[fetchRowCount - 1]["equ_no"] = v_equ_no;
		//		bcls_ret->Tables[0].Rows[fetchRowCount - 1]["ingot_code"] = v_ingot_code;

			}
			else
			{
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["STD_FLAG"] = " ";
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["VALID_FLAG"] = "0";
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["EQU_NO"] = v_equ_no;
				bcls_ret->Tables[0].Rows[fetchRowCount - 1]["INGOT_CODE"] = v_ingot_code;
		//		bcls_ret->Tables[0].Rows[fetchRowCount-1]["std_flag"] = " ";
		//		bcls_ret->Tables[0].Rows[fetchRowCount-1]["valid_flag"] = " ";
		//		bcls_ret->Tables[0].Rows[fetchRowCount - 1]["factory_div"] = tqmts01["FACTORY_DIV"];
		//		bcls_ret->Tables[0].Rows[fetchRowCount - 1]["equ_no"] = v_equ_no;
		//		bcls_ret->Tables[0].Rows[fetchRowCount - 1]["ingot_code"] = v_ingot_code;

			}
			cmd_inq_01.Close();
		}
		cmd_inq.Close();
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
