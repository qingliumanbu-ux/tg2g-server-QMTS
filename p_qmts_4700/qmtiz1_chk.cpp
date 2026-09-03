//Dec 16 08:33//请不要修改此行
/*<remark>=========================================================
/// <summary>
/// 重新预检不通过材料信息
/// <para>获取前台传入的合同号；</para>
/// <para>调用函数f_qmtizi</para>
/// </summary>
/// <param name=" "> </param>
/// <returns>处理结果</returns>
===========================================================</remark>*/

#include "stdafx.h"
#include "tqmtiz1.h"
#include "tqmtiz0.h"


/* ***** 外部函数申明 ***** */
int f_qmtizi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//钢坯产品质量保证书质量数据预检主程序
int f_qmtizj(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//线材产品质量保证书质量数据预检主程序
int f_qmtizh(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//热轧产品质量保证书质量数据预检主程序
int f_qmtizk(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//厚板产品质量保证书质量数据预检主程序
int f_qmtiza(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//冷轧产品质量保证书质量数据预检主程序

BM2F_ENTERACE(qmtiz1_chk);

int f_qmtiz1_chk(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn)
{
    //系统日志类定义
	CTracer log(__FUNCTION__);	
	
	//程序用变量
	int doFlag=0;
	
	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	
	CString q_mat_no=" ";
	CString q_pre_check_code=" ";
	CString q_order_no=" ";
	CString q_pono=" ";
	CString q_pono_2=" ";
	CString q_st_no=" ";
	CString q_sg_sign=" ";
	CString q_sg_std=" ";
	CString q_prod_class_code=" ";
	CString sqlstr = "";
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int  v_count = 0;

	CString rl_no_certi = "";

	CTQMTIZ1 tqmtiz1(conn);
	CTQMTIZ0 tqmtiz0(conn);

	CDbCommand cmd_tqmtiz1(conn);
	CDbCommand cmd(conn);
	
	try
	{
		bcls_rec_f.Tables.Add("QMTI");  
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"MAT_NO");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"CHK_FLAG");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"ORDER_NO");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"PONO");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"REP_PONO");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"ST_NO");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"SG_SIGN");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"SG_STD");
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"PROD_CLASS_CODE");
		//增加钢坯替代预检功能	alpha:2016080303
		bcls_rec_f.Tables["QMTI"].Columns.Add(DT_STRING,"RL_NO_CERTI");

		bcls_rec_f.Tables["QMTI"].Rows.Add();

		for(int i=0;i<bcls_rec->Tables[0].Rows.get_Count();i++)
		{
		    //接收前台传入的数据
		    q_mat_no  =bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();
		    q_pre_check_code  =bcls_rec->Tables[0].Rows[i]["PRE_CHECK_CODE"].ToString().Trim();
		    q_order_no  =bcls_rec->Tables[0].Rows[i]["ORDER_NO"].ToString().Trim();
			q_pono  =bcls_rec->Tables[0].Rows[i]["PONO"].ToString().Trim();
			q_pono_2  =bcls_rec->Tables[0].Rows[i]["REP_PONO"].ToString().Trim();
			q_st_no  =bcls_rec->Tables[0].Rows[i]["ST_NO"].ToString().Trim();
			q_sg_sign  =bcls_rec->Tables[0].Rows[i]["SG_SIGN"].ToString().Trim();
			q_sg_std  =bcls_rec->Tables[0].Rows[i]["SG_STD"].ToString().Trim();
			q_prod_class_code  =bcls_rec->Tables[0].Rows[i]["PROD_CLASS_CODE"].ToString().Trim();
			//增加钢坯替代预检功能	alpha:2016080303
			if (bcls_rec->Tables[0].Columns.Contains("RL_NO_CERTI"))
			{
				rl_no_certi = bcls_rec->Tables[0].Rows[i]["RL_NO_CERTI"].ToString().Trim();

				//替代轧批号有效性校验
				sqlstr = " SELECT PONO FROM TQMTQTBC1 WHERE RL_NO = @rl_no ";
				cmd.SetCommandText(sqlstr);	
				cmd.Parameters.Set("rl_no", rl_no_certi);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					CString pono_certi = cmd.GetString(1);
					if (pono_certi != q_pono)
					{
						strcpy(s.msg,"替代轧批号对应的制造命令号"+pono_certi+"与当前轧批号不符。");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					strcpy(s.msg,"委托单中没有替代轧批号信息。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				cmd.Close();
			}

		    
			Log::Trace("", "", "q_mat_no[{0}]",(const char*)q_mat_no);
			Log::Trace("", "", "q_order_no[{0}]",(const char*)q_order_no);

			//Log::Trace("QMTI","qmtiz1_chk","q_mat_no[%s],q_pre_check_code[%s],q_order_no[%s],q_pono[%s],q_pono_2[%s],q_st_no[%s],q_sg_sign[%s],q_prod_class_code[%s]",
			//      (const char*)q_mat_no,(const char*)q_pre_check_code,(const char*)q_order_no,(const char*)q_pono,(const char*)q_pono_2,
			//	  (const char*)q_sg_sign,(const char*)q_prod_class_code);
			
			if(q_pre_check_code == "1")
			{
				tqmtiz1.PRE_CHECK_CODE = "1";
				tqmtiz1.PASS_MAKER = s.userid;
				tqmtiz1.PASS_TIME = datetimeNow;
				tqmtiz1.REC_REVISOR = s.userid;
				tqmtiz1.REC_REVISE_TIME = datetimeNow;
				tqmtiz1.MAT_NO = q_mat_no;

				tqmtiz1.Update("PRE_CHECK_CODE,PASS_MAKER,PASS_TIME,REC_REVISOR,REC_REVISE_TIME","MAT_NO");

				tqmtiz0.ERR_FLAG = "0";


			}

			//调用预检函数
			bcls_rec_f.Tables["QMTI"].Rows[0]["MAT_NO"] = q_mat_no;
			bcls_rec_f.Tables["QMTI"].Rows[0]["CHK_FLAG"] = q_pre_check_code;
			bcls_rec_f.Tables["QMTI"].Rows[0]["ORDER_NO"] = q_order_no;
			bcls_rec_f.Tables["QMTI"].Rows[0]["PONO"] = q_pono;
			bcls_rec_f.Tables["QMTI"].Rows[0]["REP_PONO"] = q_pono_2;
			bcls_rec_f.Tables["QMTI"].Rows[0]["ST_NO"] = q_st_no;
			bcls_rec_f.Tables["QMTI"].Rows[0]["SG_SIGN"] = q_sg_sign;
			bcls_rec_f.Tables["QMTI"].Rows[0]["SG_STD"] = q_sg_std;
			bcls_rec_f.Tables["QMTI"].Rows[0]["PROD_CLASS_CODE"] = q_prod_class_code;
			//增加钢坯替代预检功能	alpha:2016080303
			bcls_rec_f.Tables["QMTI"].Rows[0]["RL_NO_CERTI"] = rl_no_certi;

			if(q_order_no[0]=='A'|| q_order_no[0]=='B')
			{
				doFlag = f_qmtizi(&bcls_rec_f,&bcls_ret_f,conn);
			}
			else if(q_order_no[0]=='D')
			{
				doFlag = f_qmtizj(&bcls_rec_f,&bcls_ret_f,conn);
			}
			else if(q_order_no[0]=='I')
			{
				doFlag = f_qmtizh(&bcls_rec_f,&bcls_ret_f,conn);
			}
			else if(q_order_no[0]=='J')
			{
				doFlag = f_qmtizk(&bcls_rec_f,&bcls_ret_f,conn);
			}
			else if(q_order_no[0]=='L'||q_order_no[0]=='M'||q_order_no[0]=='N'||q_order_no[0]=='X'||
					q_order_no[0]=='T'||q_order_no[0]=='S'||q_order_no[0]=='O'||q_order_no[0]=='Q')
			{
				doFlag = f_qmtiza(&bcls_rec_f,&bcls_ret_f,conn);
			}
			else 
    		{
	    		throw CApplicationException(-1, s.msg, log.Location);
			
			}

			if(doFlag != 0)
			{    
				tpabort(0);
				tpbegin(0,0);
    		}

			//插入预检履历表
			tqmtiz0.REC_CREATOR=s.userid;
			tqmtiz0.REC_CREATE_TIME=datetimeNow;
			tqmtiz0.REC_REVISOR = tqmtiz0.REC_CREATOR;
			tqmtiz0.REC_REVISE_TIME = tqmtiz0.REC_CREATE_TIME;
			tqmtiz0.MAT_NO = q_mat_no;
			tqmtiz0.ORDER_NO = q_order_no;
			tqmtiz0.CHECK_TYPE = "1";
			tqmtiz0.PROGRAM_NAME = "qmtiz1_chk";
			tqmtiz0.PONO  = q_pono;	//履历中增加制造命令号	alpha:2016080303
			tqmtiz0.RL_NO = q_pono_2;	//履历中增加轧批号	alpha:2016080303
			tqmtiz0.OLD_RL_NO = rl_no_certi;	//履历中增加质保书代表轧批号	alpha:2016080303

			v_count =0;

			sqlstr = " SELECT COUNT(1) "
					 " FROM  TQMTIZ1 "
					 " WHERE MAT_NO	    	 = @MAT_NO ";
			cmd_tqmtiz1.SetCommandText(sqlstr);
			cmd_tqmtiz1.Parameters.Set("MAT_NO",tqmtiz0.MAT_NO);
			cmd_tqmtiz1.ExecuteReader();
			if( cmd_tqmtiz1.Read() )
			{
				v_count = cmd_tqmtiz1.GetInt32(1);
			}
			cmd_tqmtiz1.Close();

			Log::Trace("", "","v_count={0}doFlag={1}q_pre_check_code={2}！",v_count,doFlag,(const char*)q_pre_check_code);
			if ((v_count > 0 || doFlag != 0) && q_pre_check_code != "1")
			{
				tqmtiz0.ERR_FLAG = "1"; //0:预检通过，1预检不通过:
				sprintf(s.msg,_S("预检未过，请查预检不通过画面。")/*预检未过，请查预检不通过画面。*/);
				tqmtiz0.DESC = s.msg;
			}
			else if(doFlag != 0 && q_pre_check_code == "1")
			{
				tqmtiz0.ERR_FLAG = "0";
				strcat(s.msg,"放行不成功。");/*放行不成功。*/
				tqmtiz0.DESC = s.msg;
			}
			else
			{
				tqmtiz0.ERR_FLAG = "0";
				sprintf(s.msg,_S("预检或放行成功。")/*预检成功。*/);
				tqmtiz0.DESC = s.msg;
			}

			tqmtiz0.Delete();
			tqmtiz0.Insert();

			tpcommit(0);
			tpbegin(0,0);
		}

		/*.print();*/
		//sprintf(s.msg,"重新预检成功！");	
	}
	
	//捕获数据库操作异常
	catch(CDbException& ex)  
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}

	//try块程序发生异常
	catch(CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//平台异常
	catch(CException& ex)
	{
		s.flag = -1;
		doFlag = -1;
	}

	s.flag = doFlag;

	return(doFlag);
}