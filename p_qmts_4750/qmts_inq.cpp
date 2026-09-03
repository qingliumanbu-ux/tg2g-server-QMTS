/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 查询制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中













/*<remark>=========================================================
/// <summary>
/// 查询各个制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)/flag(查询标记——0:表头内容;1:具体内容)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表)
///                 TQMTS01(工序制造标准表)
/// 				TQMTS02(工序成分标准表)
///                 TQMTS0X(工艺卡)
/// 前台各个QMTS0x画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各制造标准表    </param>
===========================================================</remark>*/ 

int f_qmtsp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//校验工艺卡是否已更新

// service入口
BM2F_ENTERACE(qmts_inq)


int f_qmts_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	CString QMTJBlock = " ";
	int flag = 0;
	int o_flag = 0;

	CString table_name = " ";
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CDecimal v_count = 0;
	CString catch_time = " ";

	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"st_no");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"factory_div");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"catch_time");

	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts0x("TQMTS0X");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");
	CModel tqmts03("TQMTS03");
	CModel tqmts04("TQMTS04");
	CModel tqmts05("TQMTS05");
	CModel tqmts06("TQMTS06");
	CModel tqmts07("TQMTS07");
	CModel tqmts08("TQMTS08");
	CModel tqmts0a("TQMTS0A");
	CModel tqmts0m("TQMTS0M");


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_02(conn);

	try
	{
		/*获得传入参数*/
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		flag = bcls_rec->Tables[0].Rows[0]["flag"].ToDecimal().ToInt16();
		tqmts0x["ST_NO"] = q_st_no;
		tqmts0x.Query("ST_NO");
		Log::Trace("", "","qmts_inq IN: table_name[{0}]st_no[{1}]whole_backlog_code[{2}]flag[{3}]",(const char*)table_name,(const char*)q_st_no,(const char*)q_whole_backlog_code,flag);

		if(flag == 0)
		{
			//查询出钢记号
			QMTJBlock="QMTSBLK01";
			bcls_ret->Tables.Add(QMTJBlock);   
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"st_no");
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"std_flag");      //检查标准是否存在
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"valid_flag");    //检查标准是否生效

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT DISTINCT ST_NO "
							 "  FROM " + table_name +
							 " WHERE ST_NO LIKE @st_no||'%' "
							 " ORDER BY ST_NO ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no",q_st_no.Trim());
			cmd_inq.ExecuteReader();
			fetchRowCount = 0;
			while(cmd_inq.Read())
			{
				tqmts01["ST_NO"] = cmd_inq.GetString(1);
				fetchRowCount++;

				CDataRow& row = bcls_ret->Tables[QMTJBlock].Rows.Add();
				bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["st_no"] = tqmts01["ST_NO"];

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT IF_PASS"
								 "  FROM TQMTS01 "
								 " WHERE "
								 "     ST_NO = @st_no "
								 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "; 
						break;
				}
				cmd_inq_01.SetCommandText(sqlstr);
				cmd_inq_01.Parameters.Set("st_no",tqmts01["ST_NO"].ToString());
				cmd_inq_01.Parameters.Set("whole_backlog_code",q_whole_backlog_code);
				cmd_inq_01.ExecuteReader();
				if(cmd_inq_01.Read())
				{
					tqmts01["IF_PASS"] = cmd_inq_01.GetString(1);
					bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["std_flag"] = "1";
					bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["valid_flag"] = tqmts01["IF_PASS"];
				}
				else
				{
					bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["std_flag"] = " ";
					bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["valid_flag"] = " ";
				}
				cmd_inq_01.Close();
			}
			cmd_inq.Close();

			//从工艺卡表(TQMTS0X)中查询新出钢记号
			QMTJBlock="QMTSBLK02";
			bcls_ret->Tables.Add(QMTJBlock);       
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"st_no");
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"std_flag");
			if(table_name=="tqmts08")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT DISTINCT ST_NO "
								 " FROM TQMTS0X "
								 " WHERE ST_NO NOT IN (SELECT DISTINCT st_no FROM " + table_name + ") "
								 " AND IC_CC_FLAG = 'C' "
								 " ORDER BY ST_NO ASC ";
						break;
				}
			}
			else if(table_name=="tqmts0m")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT DISTINCT ST_NO "
								 " FROM TQMTS0X "
								 " WHERE ST_NO NOT IN (SELECT DISTINCT st_no FROM " + table_name + ") "
								 " AND IC_CC_FLAG = 'I' "
								 " ORDER BY ST_NO ASC ";
						break;
				}
			}
			else
			{
					switch(conn->DatabaseKind)
					{
						case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
						case DB_KIND_MSSQL:				// MS SQL Server数据库
						case DB_KIND_ORACLE:	        // Oracle 数据库
						default:						// 所有数据库适用，通用SQL语句
							sqlstr = "SELECT DISTINCT ST_NO "
									 " FROM TQMTS0X "
									 " WHERE ST_NO NOT IN (SELECT DISTINCT st_no FROM " + table_name + ") "
									 " ORDER BY ST_NO ASC ";
							break;
					}
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			fetchRowCount=0;
			while(cmd_inq.Read())
			{
				tqmts0x["ST_NO"] = cmd_inq.GetString(1);
				fetchRowCount++;

				CDataRow& row = bcls_ret->Tables[QMTJBlock].Rows.Add();
				bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["st_no"] = tqmts0x["ST_NO"];

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS01 "
								 " WHERE ST_NO = @st_no "
								 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
						break;
				}
				cmd_inq_01.SetCommandText(sqlstr);
				cmd_inq_01.Parameters.Set("st_no",tqmts0x["ST_NO"].ToString());
				cmd_inq_01.Parameters.Set("whole_backlog_code",q_whole_backlog_code);
				v_count = cmd_inq_01.ExecuteScalar();
				if(v_count == 0)
				{
					bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["std_flag"] = " ";
				}
				else 
					bcls_ret->Tables[QMTJBlock].Rows[fetchRowCount-1]["std_flag"] = v_count.ToString();
				cmd_inq_01.Close();
			}
			cmd_inq.Close();
		}
		else if(flag > 0)
		{
			if(q_st_no.TrimOrBlank()==" ")
			{
				strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//获取制造标准内容
			QMTJBlock="QMTSBLK01";
			bcls_ret->Tables.Add(QMTJBlock);
			if(table_name == "tqmts03")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts03);
			else if(table_name == "tqmts04")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts04);
			else if(table_name == "tqmts05")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts05);
			else if(table_name == "tqmts06")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts06);
			else if(table_name == "tqmts07")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts07);
			else if(table_name == "tqmts08")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts08);
			else if(table_name == "tqmts0a")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts0a);
			else if(table_name == "tqmts0m")
				bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts0m);


			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr= "SELECT * "
							"  FROM " + table_name +
							" WHERE ST_NO = @st_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no",q_st_no);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[QMTJBlock]);
			cmd_inq.Close();

			//获取成分标准
			QMTJBlock="QMTSBLK02";
			bcls_ret->Tables.Add(QMTJBlock);
			bcls_ret->Tables[QMTJBlock].Columns.Add(tqmts02);

			fetchRowCount = 0;
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT * "
							 "  FROM TEP0002 "
							 " WHERE CODE_CLASS = 'QMYS' "
							 "   AND TRIM(CODE_DESC_2_CONTENT) IS NOT null "
							 " ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while(cmd_inq.Read())
			{ 
				cmd_inq.Fetch(tep0002);
				fetchRowCount ++;

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT * "
								 "  FROM TQMTS02 "
								 " WHERE ST_NO = @st_no"
								 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code"
								 "   AND ELM_CODE = @elm_code";
						break;
				}
				cmd_inq_02.SetCommandText(sqlstr);
				cmd_inq_02.Parameters.Set("st_no",q_st_no);
				cmd_inq_02.Parameters.Set("whole_backlog_code",q_whole_backlog_code);
				cmd_inq_02.Parameters.Set("elm_code",tep0002["CODE"].ToString());
				cmd_inq_02.ExecuteReader();
				if(cmd_inq_02.Read())
				{
					cmd_inq_02.Fetch(tqmts02);
				}
				else
				{
					tqmts02["ELM_CODE"]			= tep0002["CODE"];
					tqmts02["ELM_NAME"]			= tep0002["CODE_DESC_1_CONTENT"];
					tqmts02["MAIN_MIN"]			= 0;
					tqmts02["MAIN_MAX"]			= 99.999;
					tqmts02["MAIN_AIM"]			= 0;
					tqmts02["SPE_MIN"]				= 0;
					tqmts02["SPE_MAX"]             = 99.999;
					tqmts02["SMELT_CHEMI_FLAG"]	= "";
					tqmts02["ELM_ACCU"]			= 0;
				}
				cmd_inq_02.Close();
				CDataRow& row = bcls_ret->Tables[QMTJBlock].Rows.Add();   //新增空行
				row.Merge(tqmts02);   //将实体类的值写入新增行中
			}
			cmd_inq.Close();

			//提示信息
			QMTJBlock="QMTSBLK03";
			bcls_ret->Tables.Add(QMTJBlock);
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"if_pass");
			bcls_ret->Tables[QMTJBlock].Columns.Add(DT_STRING,"if_update");
			CDataRow& row = bcls_ret->Tables[QMTJBlock].Rows.Add();

			//校验工艺卡内容是否更新
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr= "SELECT CATCH_TIME "
							"  FROM " + table_name +
							" WHERE ST_NO = @st_no ";
					break;
			}
			Log::Trace("", "","sqlstr = [{0}]",(const char*)sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no",q_st_no);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				catch_time = cmd_inq.GetString(1);
			}
			Log::Trace("", "","catch_time = [{0}]",(const char*)catch_time);
			cmd_inq.Close();
			bcls_rec_f.Tables[0].Rows.Add();
			bcls_rec_f.Tables[0].Rows[0]["st_no"] = q_st_no;
			bcls_rec_f.Tables[0].Rows[0]["factory_div"] =tqmts0x["FACTORY_DIV"];
			bcls_rec_f.Tables[0].Rows[0]["catch_time"] = catch_time;
			doFlag = f_qmtsp_00(&bcls_rec_f,&bcls_ret_f,conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			o_flag = bcls_ret_f.Tables[0].Rows[0]["flag"];
			if (o_flag == 1)
				bcls_ret->Tables[QMTJBlock].Rows[0]["if_update"] = "工艺卡内容已更新！请根据需要重新获取工艺卡。";	
			else
				bcls_ret->Tables[QMTJBlock].Rows[0]["if_update"] = " ";

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT IF_PASS "
							 "  FROM TQMTS01 "
							 " WHERE ST_NO = @st_no "
							 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no",q_st_no);
			cmd_inq.Parameters.Set("whole_backlog_code",q_whole_backlog_code);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts01["IF_PASS"] = cmd_inq.GetString(1);
			}

			if(tqmts01["IF_PASS"].ToString() == "1")
			{
				bcls_ret->Tables[QMTJBlock].Rows[0]["if_pass"] = "制造标准已生效！";
			}
			else 
				bcls_ret->Tables[QMTJBlock].Rows[0]["if_pass"] = " ";
		}
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
