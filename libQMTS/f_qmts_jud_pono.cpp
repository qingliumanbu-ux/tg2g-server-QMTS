/*=========================================================================
//程序名称:		f_qmts_jud_pono
//隶属子系统:	QMTS
//产品名称:		
//创建人员:     
//创建时间:     2013-06-01
//修改人员:
//修改日期:
//-----------------------------------------------------------------------
//功能描述:		炉次判定函数
//数据库表:     TQMTS23
//
//主调用函数:   qmts21m_jud() - 炉次终判
//              pssm31_ht_confm() - 炉次确定
//              pssm31_ht_frcfm() - 炉次强制确定YY
//需调用函数:   f_qmts_rep_elm_confm
//-----------------------------------------------------------------------
//函数功能:     置炉次决定与最终出钢记号
//传入参数:     制造命令号, 炉次决定出钢记号、最终出钢记号
//传出参数:     成功标记
//处理流程:     1.获取/检查传入参数;
//              2.
//=========================================================================*/



//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中
/*  外部函数申明  */

BM2_FUNCTION_IMPORT
int f_qmts_rep_elm_confm(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//炉次代表成分确定
//BM2_FUNCTION_IMPORT
//int f_pssm_fin_stno_upd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //计划模块炉次最终出钢记号确定
BM2_FUNCTION_IMPORT
int f_qmts_rep_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//代表成分选择函数
int f_qmts_jud_pono(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int			doFlag							= 0		;
	int			i								= 0		;
	int			fetchRowCount					= 0		;
	EIClass bcls_rec_f1;  //调用炼钢计划函数用
	EIClass bcls_ret_f1;
	EIClass bcls_rec_f2;  //调用炉次代表成分确定函数用
	EIClass bcls_ret_f2;
	/*使用的表结构变量*/
	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");
	CModel tqmts29("TQMTS29");
	CModel tqmts21("TQMTS21");

	CString rep_elm_sel_flag = " ";  //代表样标记
	CString rep_elm_sel_time = " ";
	CString main_backlog_code = "B";
	CString pono = " ";                    //制造命令号
	CString heat_no = " ";
	CString st_sample_no = " ";          //炉次代表样号
	CString prec_st_no = " ";        //炉次预定出钢记号
	CString deci_st_no = " ";        //炉次决定出钢记号
	CString fin_st_no = " ";        //炉次最终出钢记号
	CString yy000000_cause = " ";        //YY判定原因
	CString re_check_flag = " ";        //复检标志

	CString sqlstr = " ";
	try{

		//调用函数输入块定义
		bcls_rec_f1.AddColName(1, "main_backlog_code");
		bcls_rec_f1.AddColName(1, "pono");
		bcls_rec_f1.AddColName(1, "heat_no");
		bcls_rec_f1.AddColName(1, "deci_st_no");
		bcls_rec_f1.AddColName(1, "fin_st_no");
		bcls_rec_f1.AddColName(1, "re_check_flag");
		bcls_rec_f1.AddColName(1, "rep_elm_sel_flag");
		bcls_rec_f1.AddColName(1, "rep_elm_sample_no");
		bcls_rec_f1.AddColName(1, "rep_elm_sel_time");

		bcls_rec_f2.AddColName(1, "main_backlog_code");
		bcls_rec_f2.AddColName(1, "pono");
		bcls_rec_f2.AddColName(1, "st_no");
		bcls_rec_f2.AddColName(1, "st_sample_no");
		bcls_rec_f2.AddColName(1, "heat_no");

		/*获得传入参数*/
		main_backlog_code = bcls_rec->Tables[0].Rows[0]["main_backlog_code"].ToString();	 // 主工序代码
		pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString();		         // 制造命令号
		heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString();
		st_sample_no = bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString();
		prec_st_no = bcls_rec->Tables[0].Rows[0]["prec_st_no"].ToString();
		deci_st_no = bcls_rec->Tables[0].Rows[0]["deci_st_no"].ToString();
		fin_st_no = bcls_rec->Tables[0].Rows[0]["fin_st_no"].ToString();
		yy000000_cause = bcls_rec->Tables[0].Rows[0]["yy000000_cause"].ToString();
		re_check_flag = bcls_rec->Tables[0].Rows[0]["RE_CHECK_FLAG"].ToString();

		CDbCommand cmd_inq(conn);
		CDbCommand cmd1(conn);
		Log::Trace("", "", "f_qmts_jud_pono 传入参数st_sample_no={0}", st_sample_no);
		//1-选择代表样
		if (st_sample_no.Trim() == "")
		{
			//无代表样,置空标记
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "UPDATE TQMTS24 "
					" SET REP_ELM_SEL_FLAG = ' ' "
					" WHERE  PONO = @pono ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("main_backlog_code", main_backlog_code);
			cmd_inq.Parameters.Set("pono", pono);
			cmd_inq.ExecuteReader();
			cmd_inq.Close();

			//代表样选定标记为空
			rep_elm_sel_flag = " ";
			Log::Trace("", "", "代表样选定标记为空rep_elm_sel_flag={0}", rep_elm_sel_flag);
		}
		else
		{
			tqmts24["HEAT_NO"] = heat_no;
			tqmts24["ST_SAMPLE_NO"] = st_sample_no;
			if (tqmts24.QueryCount("HEAT_NO,ST_SAMPLE_NO")<=0)
			{
				sprintf(s.msg, "制造命令号[%s]的试样号[%s]不存在!", (const char*)pono, (const char*)st_sample_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT * "
					" FROM TQMTS24 "
					" WHERE PONO = @pono "
					" AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("main_backlog_code", main_backlog_code);
			cmd_inq.Parameters.Set("pono", pono);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("st_sample_no", st_sample_no);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts24);
			}
			cmd_inq.Close();
			Log::Trace("", "", "清除代表样");
			//清除代表样
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "UPDATE TQMTS24 "
					" SET REP_ELM_SEL_FLAG = ' ' "
					" WHERE  PONO = @pono "
					" AND ST_SAMPLE_DIV = '2' ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("main_backlog_code", main_backlog_code);
			cmd_inq.Parameters.Set("pono", pono);
			cmd_inq.ExecuteReader();
			cmd_inq.Close();

			//选定代表样
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "UPDATE TQMTS24 "
					" SET REP_ELM_SEL_FLAG = '1' "
					" WHERE  PONO = @pono "
					" AND HEAT_NO = @heat_no "
					" AND ST_SAMPLE_NO = @st_sample_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("main_backlog_code", main_backlog_code);
			cmd_inq.Parameters.Set("pono", pono);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("st_sample_no", st_sample_no);
			cmd_inq.ExecuteReader();
			cmd_inq.Close();


			//置代表样选定标记
			rep_elm_sel_flag= "1";
			Log::Trace("", "", "11代表样选定标记为空rep_elm_sel_flag={0}", rep_elm_sel_flag);
		}

		

		//2-调用炼钢计划函数,置出钢记号等
		Log::Trace("", "", "2-调用炼钢计划函数,置出钢记号等");
		rep_elm_sel_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
		if (fin_st_no.Trim() == "")
		{
			if (deci_st_no.Trim() != "")
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "UPDATE TQMTS23 "
						" SET DECI_ST_NO = @deci_st_no,YY_CAUSE = @yy000000_cause "
						" WHERE HEAT_NO = @heat_no ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.ExecuteReader();
				cmd_inq.Close();

				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "UPDATE TPSSM11 "
						" SET REP_ELM_SEL_FLAG = @rep_elm_sel_flag "
						" WHERE HEAT_NO = @heat_no ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", heat_no);
				cmd_inq.Parameters.Set("rep_elm_sel_flag", rep_elm_sel_flag);
				cmd_inq.ExecuteReader();
				cmd_inq.Close();
			}
		}
		else
		{
			//bcls_rec_f1.Tables[0].Rows[0]["main_backlog_code"] = main_backlog_code;
			//bcls_rec_f1.Tables[0].Rows[0]["pono"] = main_backlog_code;
			//bcls_rec_f1.Tables[0].Rows[0]["deci_st_no"] = deci_st_no;
			//bcls_rec_f1.Tables[0].Rows[0]["fin_st_no"] = fin_st_no;
			//bcls_rec_f1.Tables[0].Rows[0]["rep_elm_sel_flag"] = rep_elm_sel_flag;
			//if ("1" == re_check_flag) 
			//	re_check_flag= "2";
			//bcls_rec_f1.Tables[0].Rows[0]["re_check_flag"] = re_check_flag;//复检标记
			//bcls_rec_f1.Tables[0].Rows[0]["rep_elm_sample_no"] = st_sample_no;
			//bcls_rec_f1.Tables[0].Rows[0]["rep_elm_sel_time"] = rep_elm_sel_time;
			//doFlag = f_pssm_fin_stno_upd(&bcls_rec_f1, &bcls_ret_f1, conn);
			//if (doFlag != 0)
			//{
			//	strcat(s.msg, _RES("f_pssm_fin_stno_upd执行失败。"));
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			Log::Trace("", "", "产品化只更新TQMTS23表");
			//产品化只更新TQMTS23表
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "UPDATE TQMTS23 "
					" SET FIN_ST_NO = @fin_st_no "
					" WHERE HEAT_NO = @heat_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.Parameters.Set("fin_st_no", fin_st_no);
			cmd_inq.ExecuteReader();
			cmd_inq.Close();

		}


		//3-炉次代表成分确定
		Log::Trace("", "", "3-炉次代表成分确定");
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND REP_ELM_SEL_FLAG ='1' fetch first 1 rows only ";//代表成分标记
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts24);
		}
		else
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "DELETE FROM TQMTS29 WHERE PONO = @pono ";
				break;
			}
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("pono", pono);
			cmd1.ExecuteNonQuery();
			cmd1.Close();
		}
		cmd_inq.Close();

		//代表成分写入
		if (st_sample_no.Trim()!="")
		{
			bcls_rec_f2.Tables[0].Rows.Add();
			bcls_rec_f2.Tables[0].Rows[0]["pono"] = pono;
			if (fin_st_no.Trim() == "")
			{
				bcls_rec_f2.Tables[0].Rows[0]["st_no"] = deci_st_no;  //出钢记号为决定出钢记号
			}
			else
			{
				bcls_rec_f2.Tables[0].Rows[0]["st_no"] = fin_st_no;  //出钢记号为最终出钢记号
			}
			bcls_rec_f2.Tables[0].Rows[0]["st_sample_no"] = st_sample_no;
			bcls_rec_f2.Tables[0].Rows[0]["heat_no"] = heat_no;
			doFlag = f_qmts_rep_upd(&bcls_rec_f2, &bcls_ret_f2, conn);
			if (doFlag != 0)
			{
				strcat(s.msg, "f_qmts_rep_upd执行失败。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		

		Log::Trace("", "", "4-置板坯决定和最终出钢记号");
		//4-置板坯决定和最终出钢记号
		//switch (conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = "UPDATE  TMMSM01 A "
		//		" SET     ST_NO = DECODE(@fin_st_no, ' ', ST_NO, DECODE(FIN_ST_NO, ' ', @fin_st_no, FIN_ST_NO)), "
		//		" DECI_ST_NO = @deci_st_no, "
		//		" FIN_ST_NO = DECODE(FIN_ST_NO, ' ', @fin_st_no, FIN_ST_NO), "
		//		" CHG_ST_NO_TYPE = DECODE(:fin_st_no, @prec_st_no, CHG_ST_NO_TYPE, ' ', CHG_ST_NO_TYPE, DECODE(CHG_ST_NO_TYPE, ' ', '5', CHG_ST_NO_TYPE)) "
		//		//" STOCK_NO = NVL((select USAGE_TYPE from TQMTS08 B where B.ST_NO = DECODE(@fin_st_no, ' ', A.ST_NO, @fin_st_no)), ' ') "
		//		" WHERE  PONO = @pono ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("heat_no", heat_no);
		//cmd_inq.Parameters.Set("fin_st_no", fin_st_no);
		//cmd_inq.Parameters.Set("deci_st_no", deci_st_no);
		//cmd_inq.Parameters.Set("prec_st_no", prec_st_no);
		//cmd_inq.ExecuteNonQuery();
		//cmd_inq.Close();
		

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		
		//CFormattable arguments[] = { ex.GetCode() };
		//CMessageFormat::Format(s.msg,  "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		//sprintf(s.msg, "sql[%s]erorr[%s]", (const char*)sqlstr, (const char*)ex.GetMsg());
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
		strncpy(s.sysmsg, "System Exception", sizeof(s.sysmsg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}
