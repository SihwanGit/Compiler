// Sint.java
// Interpreter for S
package Lab3;
import java.util.Scanner;

public class Sint {
    static Scanner sc = new Scanner(System.in);
    static State state = new State();

    State Eval(Command c, State state) { 
        if (c instanceof Decl) {
            Decls decls = new Decls();
            decls.add((Decl) c);
            return allocate(decls, state);
        }
        if (c instanceof Stmt)
            return Eval((Stmt) c, state); 
		
	    throw new IllegalArgumentException("no command");
    }
  
    State Eval(Stmt s, State state) {
        if (s instanceof Empty) 
	        return Eval((Empty)s, state);
        if (s instanceof Assignment)  
	        return Eval((Assignment)s, state);
        if (s instanceof If)  
	        return Eval((If)s, state);
        if (s instanceof While)  
	        return Eval((While)s, state);
        if (s instanceof Stmts)  
	        return Eval((Stmts)s, state);
	    if (s instanceof Let)  
	        return Eval((Let)s, state);
	    if (s instanceof Read)  
	        return Eval((Read)s, state);
	    if (s instanceof Print)  
	        return Eval((Print)s, state);
        throw new IllegalArgumentException("no statement");
    }

    State Eval(Empty s, State state) {
        return state;
    }
  
    State Eval(Assignment a, State state) { 
        Value v = V(a.expr, state);
	    return state.set(a.id, v);
    }

    State Eval(Read r, State state) {
        if (r.id.type == Type.INT) {
	        int i = sc.nextInt();
	        state.set(r.id, new Value(i));
	    } 

	    if (r.id.type == Type.BOOL) {
	        boolean b = sc.nextBoolean();	
            state.set(r.id, new Value(b));
	    }
	    
	    if (r.id.type == Type.STRING) {
	        String s = (String) sc.next();
	        state.set(r.id, new Value(s));
	    }
		
	    return state;
    }

    State Eval(Print p, State state) {
	    System.out.println(V(p.expr, state));
        return state; 
    }
  
    State Eval(Stmts ss, State state) {
        for (Stmt s : ss.stmts) {
            state = Eval(s, state);
        }
        return state;
    }
  
    State Eval(If c, State state) {
        if (V(c.expr, state).boolValue( ))
            return Eval(c.stmt1, state);
        else
            return Eval(c.stmt2, state);
    }
 
    State Eval(While l, State state) {
        if (V(l.expr, state).boolValue( ))
            return Eval(l, Eval(l.stmt, state));
        else 
	        return state;
    }

    State Eval(Let l, State state) {
        State s = allocate(l.decls, state);
        s = Eval(l.stmts,s);
	    return free(l.decls, s);
    }
    
    State allocate (Decls ds, State state) {
        // add entries for declared variables (ds) on the state
        /* student exercise */
        if (ds != null) {
            for (Decl decl : ds) {
                if (decl.expr == null) {
                    // TODO: [push a new variable with id and default Value]
                    // expr이 없는 할당문 (초기화)
                    Value defaultValue = null;

                    // 변수 타입에 맞는 기본값을 설정
                    if (decl.type == Type.INT) {
                        defaultValue = new Value(0); // 기본값 0
                    } else if (decl.type == Type.BOOL) {
                        defaultValue = new Value(false); // 기본값 false
                    } else if (decl.type == Type.STRING) {
                        defaultValue = new Value(""); // 기본값 빈 문자열
                    }
                    // 기본값을 상태에 추가
                    state = state.set(decl.id, defaultValue);
                } else {
                    // TODO: [push a new variable with id and value of V(expr)]
                    // expr이 있는 할당문 (값 할당)
                    Value value = V(decl.expr, state); // expr을 평가하여 값 계산
                    // 계산된 값을 상태에 추가
                    state.set(decl.id, value);
                }
            }
        }
        return state;
    }

    State free (Decls ds, State state) {
    	// free the entries for declared variables (ds) from the state
    	/* student exercise */
    	if (ds != null) {
    		for (Decl decl : ds) {
    			// TODO: [pop a variable located at the top of stack]
    			 state.remove(decl.id);
    		}
    		return state;
    	}
        return null;
    }

    Value binaryOperation(Operator op, Value v1, Value v2) { // Lab03
        check(!v1.undef && !v2.undef,"reference to undef value");
	    switch (op.val) {
	    case "+":
            return new Value(v1.intValue() + v2.intValue());
        case "-": 
            return new Value(v1.intValue() - v2.intValue());
        case "*": 
            return new Value(v1.intValue() * v2.intValue());
        case "/": 
            return new Value(v1.intValue() / v2.intValue());
            // TODO: [Add relational & logical operations]
            /* student exercise */

            //관계연산
        case "<":
            return new Value(v1.intValue() < v2.intValue());
        case "<=":
            return new Value(v1.intValue() <= v2.intValue());
        case ">":
            return new Value(v1.intValue() > v2.intValue());
        case ">=":
            return new Value(v1.intValue() >= v2.intValue());
        case "==":
            return new Value(v1.intValue() == v2.intValue());
        case "!=":
            return new Value(v1.intValue() != v2.intValue());
            
            //논리연산
        case "&":
            return new Value(v1.boolValue() && v2.boolValue());
        case "|":
            return new Value(v1.boolValue() || v2.boolValue());

	    default:
	        throw new IllegalArgumentException("no operation");
	    }
    } 
    
    Value unaryOperation(Operator op, Value v) {
        check( !v.undef, "reference to undef value");
	    switch (op.val) {
        case "!": 
            return new Value(!v.boolValue( ));
        case "-": 
            return new Value(-v.intValue( ));
        default:
            throw new IllegalArgumentException("no operation: " + op.val); 
        }
    } 

    static void check(boolean test, String msg) {
        if (test) return;
        System.err.println(msg);
    }

    Value V(Expr e, State state) {
        if (e instanceof Value) 
            return (Value) e;

        if (e instanceof Identifier) {
	        Identifier v = (Identifier) e;
            return (Value)(state.get(v));
	    }

        if (e instanceof Binary) {
            Binary b = (Binary) e;
            Value v1 = V(b.expr1, state);
            Value v2 = V(b.expr2, state);
            return binaryOperation (b.op, v1, v2); 
        }

        if (e instanceof Unary) {
            Unary u = (Unary) e;
            Value v = V(u.expr, state);
            return unaryOperation(u.op, v); 
        }
        throw new IllegalArgumentException("no operation");
    }

    public static void main(String args[]) {
	    if (args.length == 0) {
	        Sint sint = new Sint(); 
			Lexer.interactive = true;
            System.out.println("Language S Interpreter 2.0");
            System.out.print(">> ");
	        Parser parser  = new Parser(new Lexer());

	        do { // Program = Command*
	            if (parser.token == Token.EOF)
		            parser.token = parser.lexer.getToken();
	       
	            Command command=null;
                try {
	                command = parser.command();
	                if (command != null) command.display(0);    // display AST
	                else
						 throw new Exception();
                } catch (Exception e) {
                    System.out.println(e);
		            System.out.print(">> ");
                    continue;
                }

	            if (command.type != Type.ERROR) {
                    System.out.println("\nInterpreting..." );
                    try {
                        state = sint.Eval(command, state);
                    } catch (Exception e) {
                         System.err.println(e);  
                    }
                }
		    System.out.print(">> ");
	        } while (true);
	    }
        else {
	        System.out.println("Begin parsing... " + args[0]);
	        Command command = null;
	        Parser parser  = new Parser(new Lexer(args[0]));
	        Sint sint = new Sint();

	        do {	// Program = Command*
	            if (parser.token == Token.EOF)
                    break;
	         
                try {
	                command = parser.command();
	                if (command != null) command.display(0);    // display AST
	                else
	                	throw new Exception();
                } catch (Exception e) {
                    System.out.println(e);
                    continue;
                }

	            if (command.type!=Type.ERROR) {
                    System.out.println("\nInterpreting..." + args[0]);
                    try {
                        state = sint.Eval(command, state);
                    } catch (Exception e) {
                        System.err.println(e);  
                    }
                }
	        } while (command != null);
        }        
    }
}