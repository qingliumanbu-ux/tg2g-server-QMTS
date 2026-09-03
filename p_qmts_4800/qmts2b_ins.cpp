/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-26
Description: 板坯钻样成分实绩信息新增
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


#include "CUtils.h"



/*  外部函数申明  */
int f_qmts_jud(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);//判定


/*<remark>=========================================================
/// <summary>
///板坯钻样成分实绩信息新增
/// <para>
/// 1.板坯钻样成分实绩信息新增
/// 
/// </para>
/// <para>数据库表：TQMTS2B(实绩_板坯成分主信息)		</para>
/// <para>          TQMTS2C(实绩_板坯成分)		        </para>
/// <para>主调用函数：前台QMTS2B画面的F3(新增)调用		</para>
/// <para>需调用函数：							        </para>
/// </summary>
/// <param name="ST_SAMPLE_NO">  试样号 				</param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(qmts2b_ins)

int f_qmts2b_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal st_sample_seq = 0;
	int elm_num = 0;//实绩元素数量
	int i_count = 0;
	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts2b("TQMTS2B");
	CModel tqmts2c("TQMTS2C");
	CModel tep0002("TEP0002");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{  
 		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{
			/* 取得单行传入信息 */
			tqmts2b.Reset();
			tqmts2b.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","tqmts2c_upd IN:---SLAB_NO[{0}]",(const char*)tqmts2b["SLAB_NO"].ToString());
			Log::Trace("", "","tqmts2c_upd IN:---SAMPLE_POS_CODE[{0}]",(const char*)tqmts2b["SAMPLE_POS_CODE"].ToString());
			
			//校验传入参数
			if(tqmts2b["SLAB_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts2b["SAMPLE_POS_CODE"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004341")/*取样位置不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts2b["REC_CREATOR"] = s.userid;
			tqmts2b["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts2b["REC_REVISOR"] = " ";
			tqmts2b["REC_REVISE_TIME"] = " ";
			tqmts2b["ARCHIVE_FLAG"] = " ";

			tqmts2b["ANALYSE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts2b["JUDGE_CODE"] = " ";
			tqmts2b["ELM_BACKUP"] = 2;


			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT PONO,HEAT_NO,ST_NO,SAMPLE_POS,SAMPLE_MODE_CODE,SAMPLE_MODE,REC_CREATE_TIME "
							 "  FROM TQMTS2A "
							 " WHERE SLAB_NO = @tqmts2b.SLAB_NO "
							 "   AND SAMPLE_POS_CODE = @tqmts2b.SAMPLE_POS_CODE ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tqmts2b.SLAB_NO", tqmts2b["SLAB_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("tqmts2b.SAMPLE_POS_CODE", tqmts2b["SAMPLE_POS_CODE"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts2b["PONO"] = cmd_inq.GetString(1);
				tqmts2b["HEAT_NO"] = cmd_inq.GetString(2);
				tqmts2b["ST_NO"] = cmd_inq.GetString(3);
				tqmts2b["SAMPLE_POS"] = cmd_inq.GetString(4);
				tqmts2b["SAMPLE_MODE_CODE"] = cmd_inq.GetString(5);
				tqmts2b["SAMPLE_MODE"] = cmd_inq.GetString(6);
				tqmts2b["SAMPLE_TIME"] = cmd_inq.GetString(7);
			}
			else
			{
				sprintf(s.msg,"材料号[%s]取样位置[%s]未下委托，不可录入实绩。",(const char*)tqmts2b["SLAB_NO"].ToString(),(const char*)tqmts2b["SAMPLE_POS_CODE"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
			Log::Trace("", "","tqmts2b.PONO = [{0}]", (const char*)tqmts2b["PONO"].ToString());
			Log::Trace("", "","tqmts2b.HEAT_NO = [{0}]", (const char*)tqmts2b["HEAT_NO"].ToString());
			Log::Trace("", "","tqmts2b.ST_NO = [{0}]", (const char*)tqmts2b["ST_NO"].ToString());
			Log::Trace("", "","tqmts2b.SAMPLE_POS = [{0}]", (const char*)tqmts2b["SAMPLE_POS"].ToString());
			Log::Trace("", "","tqmts2b.SAMPLE_MODE_CODE = [{0}]", (const char*)tqmts2b["SAMPLE_MODE_CODE"].ToString());
			Log::Trace("", "","tqmts2b.SAMPLE_MODE = [{0}]", (const char*)tqmts2b["SAMPLE_MODE"].ToString());



			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = "SELECT count(ST_SAMPLE_NO) "
					"  FROM TQMTS2B "
					" WHERE SLAB_NO = @tqmts2b.SLAB_NO "
					"   AND SAMPLE_POS_CODE = @tqmts2b.SAMPLE_POS_CODE ";
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tqmts2b.SLAB_NO", tqmts2b["SLAB_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("tqmts2b.SAMPLE_POS_CODE", tqmts2b["SAMPLE_POS_CODE"].ToString().Trim());

			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				i_count = cmd_inq.GetInt32(1);
			}

			cmd_inq.Close();
			if (i_count > 0)
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT ST_SAMPLE_NO "
						"  FROM TQMTS2B "
						" WHERE SLAB_NO = @tqmts2b.SLAB_NO "
						"   AND SAMPLE_POS_CODE = @tqmts2b.SAMPLE_POS_CODE "
					    "  ORDER BY ST_SAMPLE_NO ";

					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tqmts2b.SLAB_NO", tqmts2b["SLAB_NO"].ToString().Trim());
				cmd_inq.Parameters.Set("tqmts2b.SAMPLE_POS_CODE", tqmts2b["SAMPLE_POS_CODE"].ToString().Trim());
				cmd_inq.ExecuteReader();
				Log::Trace("", "", "sqlstr[{0}]", sqlstr);
				while (cmd_inq.Read())
				{
					st_sample_seq = st_sample_seq + 1;
					Log::Trace("", "", "st_sample_seq[{0}]", st_sample_seq);
				}
				cmd_inq.Close();
				st_sample_seq = st_sample_seq + 1;
			}
			else
			{
				st_sample_seq = 1;
				Log::Trace("", "", "st_sample_seq[{0}]", st_sample_seq.ToInt32());
			}



			//拼试样号

			tqmts2b["ST_SAMPLE_NO"] = tqmts2b["SLAB_NO"].ToString() + tqmts2b["SAMPLE_POS_CODE"].ToString() + st_sample_seq.ToString().Trim();
			Log::Trace("", "", "ST_SAMPLE_NO[{0}]", (const char*)tqmts2b["ST_SAMPLE_NO"].ToString());


			Log::Trace("", "","INSERT TQMTS2C");
			tqmts2c["REC_CREATOR"]			= tqmts2b["REC_CREATOR"];
			tqmts2c["REC_CREATE_TIME"]		= tqmts2b["REC_CREATE_TIME"];
			tqmts2c["REC_REVISOR"]			= tqmts2b["REC_REVISOR"];
			tqmts2c["REC_REVISE_TIME"]		= tqmts2b["REC_REVISE_TIME"];
			tqmts2c["ARCHIVE_FLAG"]		= tqmts2b["ARCHIVE_FLAG"];
			tqmts2c["ST_SAMPLE_NO"]		= tqmts2b["ST_SAMPLE_NO"];
			tqmts2c["SLAB_NO"]				= tqmts2b["SLAB_NO"];
			tqmts2c["SAMPLE_POS_CODE"]		= tqmts2b["SAMPLE_POS_CODE"];
			tqmts2c["SAMPLE_POS"]			= tqmts2b["SAMPLE_POS"];
			tqmts2c["SAMPLE_MODE_CODE"]	= tqmts2b["SAMPLE_MODE_CODE"];
			tqmts2c["SAMPLE_MODE"]			= tqmts2b["SAMPLE_MODE"];
			tqmts2c["PONO"]				= tqmts2b["PONO"];
			tqmts2c["HEAT_NO"]				= tqmts2b["HEAT_NO"];
			tqmts2c["ST_NO"]				= tqmts2b["ST_NO"];
			tqmts2c["TEST_TIME"]			= tqmts2b["ANALYSE_TIME"];
			tqmts2c["ELM_OK"]				= 0;
			tqmts2c["ELM_BACKUP"]			= 2;

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
							 "   AND TRIM(CODE_DESC_2_CONTENT) is not null "
							 " ORDER BY CODE_DESC_2_CONTENT ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tep0002); 
				if (tep0002["CODE_DESC_1_CONTENT"].ToString() == 'C')
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["C_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else if (tep0002["CODE_DESC_1_CONTENT"].ToString() == "Si")
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["SI_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else if (tep0002["CODE_DESC_1_CONTENT"].ToString() == "Mn")
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["MN_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else if (tep0002["CODE_DESC_1_CONTENT"].ToString() == 'P')
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["P_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else if (tep0002["CODE_DESC_1_CONTENT"].ToString() == 'S')
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["S_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else if (tep0002["CODE_DESC_1_CONTENT"].ToString() == "Al")
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["AL_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else if (tep0002["CODE_DESC_1_CONTENT"].ToString() == 'N')
				{
					tqmts2c["ELM_VALUE"] = tqmts2b["N_VALUE"].ToString(); //根据元素代码得到前台传入的元素值
				}
				else
				{
					continue;
				}
				tep0002.Print();
				tqmts2c["ELM_ACT_OLD"] = tqmts2c["ELM_VALUE"];///update by yiling 20170802 原始值保留。
				Log::Trace("", "", "ELM_CODE[{0}], ELM_NAME[{1}], ELM_VALUE[{2}]", (const char*)tep0002["CODE"].ToString(), (const char*)tep0002["CODE_DESC_1_CONTENT"].ToString(), tqmts2c["ELM_VALUE"].ToDecimal().ToDouble());
				
				if (tqmts2c["ELM_VALUE"].ToDecimal() !=-1)
				{
					elm_num++;
					tqmts2c["ELM_CODE"] = tep0002["CODE"];
					tqmts2c["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
					tqmts2c["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
					tqmts2c["ELM_UNIT"] = "%";
					
					tqmts2c.TrimOrBlank();
					tqmts2c.Print();
					tqmts2c.Insert();
				}
			}
			cmd_inq.Close();
			Log::Trace("", "", "11111111");
	 
			if(elm_num<1)
			{
				strcpy(s.msg,_RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts2b.TrimOrBlank();
			tqmts2b.Print();
			tqmts2b.Insert();
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS2C";
			bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts2b["ST_SAMPLE_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts2b["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts2b["HEAT_NO"];

			//启动判定
			doFlag = f_qmts_jud(&bcls_rec_f,&bcls_ret_f,conn);
			if(doFlag != 0)
			{    
				Log::Trace("", "","f_qmts_jud() msg = [{0}]",s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
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

	return doFlag;
}
