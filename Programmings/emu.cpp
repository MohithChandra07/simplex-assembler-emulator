/* 
    Name      : Mohith Chandra Gugulothu 
    Roll      : 2403cs04
    Course    : Computer Architecture CS2206
    Project   : Two Pass Assembler & Emulator for SIMPLEX Instruction Set
    File Name : emu.cpp -- emulator
    Language  : C++
    Compile   : g++ emu.cpp -o emu
    Useage    : ./emu <filename>.obj  -trace -bfrafr -memdump

    All work presented here is independently developed by the author (Mohtih Chandra Gugulothu)
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

static const int MEMORY_SIZE = 65536;

enum Reg { REG_A = 0, REG_B = 1, REG_PC = 2, REG_SP = 3 };

/*  Column widths  */

static const int W_STEP    =  6;
static const int W_PC      =  8;
static const int W_INSTR   =  8;
static const int W_MNEM    =  8;
static const int W_OPERAND = 10;
static const int W_REG     =  8;
static const std::string SEP = "  ";

/*  Emulator state  */

struct Emulator {
    int  reg[4];
    int  memory[MEMORY_SIZE];
    bool running;
    int  instruction_count;

    Emulator() : running(true), instruction_count(0) {
        std::memset(reg,    0, sizeof(reg));
        std::memset(memory, 0, sizeof(memory));
    }
};


/*  Formatting helpers   */


static std::string hex8(int v)
{
    std::ostringstream o;
    o << std::hex << std::uppercase
      << std::setfill('0') << std::setw(8)
      << static_cast<unsigned int>(v);
    return o.str();
}

static std::string fmtStep(int v)
{
    std::ostringstream o;
    o << std::dec << std::right
      << std::setfill(' ') << std::setw(W_STEP) << v;
    return o.str();
}

static std::string fmtMnem(const char* s)
{
    std::ostringstream o;
    o << std::left << std::setfill(' ') << std::setw(W_MNEM) << s;
    return o.str();
}

static std::string fmtOperand(bool present, int v)
{
    std::ostringstream o;
    if (present)
        o << std::right << std::setfill(' ')
          << std::setw(W_OPERAND) << hex8(v);
    else
        o << std::setfill(' ') << std::setw(W_OPERAND) << "";
    return o.str();
}

/* divider widths */
static std::string divider_trace()
{
    int total = W_STEP
              + (int)SEP.size() + W_PC
              + (int)SEP.size() + W_INSTR
              + (int)SEP.size() + W_MNEM
              + (int)SEP.size() + W_OPERAND
              + (int)SEP.size() + W_REG
              + (int)SEP.size() + W_REG
              + (int)SEP.size() + W_REG;
    return std::string(total, '-');
}

static std::string divider_reg()
{
    int total = W_STEP
              + (int)SEP.size() + W_PC
              + (int)SEP.size() + W_REG
              + (int)SEP.size() + W_REG
              + (int)SEP.size() + W_REG;
    return std::string(total, '-');
}

/* 4 words per line: addr  W0  W1  W2  W3 */
static std::string divider_mem()
{
    /* addr(8) + sep(2) + 4*(8+1) = 8+2+36 = 46 */
    return std::string(46, '-');
}


/*  Instruction helpers                                                 */

static const char* mnemonic_of(int opcode)
{
    switch (opcode) {
        case  0: return "ldc";
        case  1: return "adc";
        case  2: return "ldl";
        case  3: return "stl";
        case  4: return "ldnl";
        case  5: return "stnl";
        case  6: return "add";
        case  7: return "sub";
        case  8: return "shl";
        case  9: return "shr";
        case 10: return "adj";
        case 11: return "a2sp";
        case 12: return "sp2a";
        case 13: return "call";
        case 14: return "return";
        case 15: return "brz";
        case 16: return "brlz";
        case 17: return "br";
        case 18: return "HALT";
        default: return "???";
    }
}

static bool has_operand(int opcode)
{
    switch (opcode) {
        case  6: case  7: case  8: case  9:
        case 11: case 12: case 14: case 18:
            return false;
        default:
            return true;
    }
}


/*  Row builders                                                        */


static std::string trace_header()
{
    std::ostringstream o;
    o << std::setfill(' ')
      << std::right << std::setw(W_STEP)    << "Step"     << SEP
      << std::left  << std::setw(W_PC)      << "PC"       << SEP
      << std::left  << std::setw(W_INSTR)   << "Instr"    << SEP
      << std::left  << std::setw(W_MNEM)    << "Mnemonic" << SEP
      << std::right << std::setw(W_OPERAND) << "Operand"  << SEP
      << std::left  << std::setw(W_REG)     << "A"        << SEP
      << std::left  << std::setw(W_REG)     << "B"        << SEP
      << std::left  << std::setw(W_REG)     << "SP";
    return o.str();
}

static std::string trace_row(int step, int pc, int instr,
                              int opcode, int operand,
                              int a, int b, int sp)
{
    std::ostringstream o;
    o << fmtStep(step)                            << SEP
      << hex8(pc)                                 << SEP
      << hex8(instr)                              << SEP
      << fmtMnem(mnemonic_of(opcode))             << SEP
      << fmtOperand(has_operand(opcode), operand) << SEP
      << hex8(a)  << SEP
      << hex8(b)  << SEP
      << hex8(sp);
    return o.str();
}

static std::string reg_header()
{
    std::ostringstream o;
    o << std::setfill(' ')
      << std::right << std::setw(W_STEP) << "Step" << SEP
      << std::left  << std::setw(W_PC)   << "PC"   << SEP
      << std::left  << std::setw(W_REG)  << "A"    << SEP
      << std::left  << std::setw(W_REG)  << "B"    << SEP
      << std::left  << std::setw(W_REG)  << "SP";
    return o.str();
}

static std::string reg_row(int step, int pc, int a, int b, int sp)
{
    std::ostringstream o;
    o << fmtStep(step) << SEP
      << hex8(pc)      << SEP
      << hex8(a)       << SEP
      << hex8(b)       << SEP
      << hex8(sp);
    return o.str();
}

static std::string reg_state_block(const Emulator& emu)
{
    std::ostringstream o;
    o << "Final register state:\n"
      << "  A  = " << hex8(emu.reg[REG_A])
      << "  (" << std::dec << emu.reg[REG_A]  << ")\n"
      << "  B  = " << hex8(emu.reg[REG_B])
      << "  (" << std::dec << emu.reg[REG_B]  << ")\n"
      << "  PC = " << hex8(emu.reg[REG_PC])
      << "  (" << std::dec << emu.reg[REG_PC] << ")\n"
      << "  SP = " << hex8(emu.reg[REG_SP])
      << "  (" << std::dec << emu.reg[REG_SP] << ")\n";
    return o.str();
}


/*  Memory dump — 4 words per line                                      */

static void write_memdump(const Emulator& emu, std::ostream& out, int icount)
{
    /* find last non-zero word */
    int last = 0;
    for (int i = MEMORY_SIZE - 1; i >= 0; --i) {
        if (emu.memory[i] != 0) { last = i; break; }
    }
    /* round up to next multiple of 4 */
    int end = ((last / 4) + 1) * 4;

    std::string div = divider_mem();

    out << "Memory Dump  (4 words per line)\n";
    out << "Instructions executed: " << std::dec << icount << "\n";
    out << div << "\n";

    /* column header */
    out << std::setfill(' ')
        << std::left << std::setw(8) << "Address" << SEP
        << std::right
        << std::setw(8) << "+0" << " "
        << std::setw(8) << "+1" << " "
        << std::setw(8) << "+2" << " "
        << std::setw(8) << "+3"
        << "\n"
        << div << "\n";

    for (int row = 0; row < end; row += 4) {
        out << hex8(row) << SEP
            << hex8(emu.memory[row + 0]) << " "
            << hex8(emu.memory[row + 1]) << " "
            << hex8(emu.memory[row + 2]) << " "
            << hex8(emu.memory[row + 3]) << "\n";
    }

    out << div << "\n\n";
    out << reg_state_block(emu);
}


/*  Load object file                                                    */

static bool load_object(Emulator& emu, const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open '" << filename << "'\n";
        return false;
    }
    int addr = 0;
    unsigned int word;
    while (file.read(reinterpret_cast<char*>(&word), sizeof(word))) {
        if (addr >= MEMORY_SIZE) {
            std::cerr << "Object file too large\n";
            return false;
        }
        emu.memory[addr++] = static_cast<int>(word);
    }
    return true;
}


/*  Execute                                                             */

static void execute(Emulator& emu,
                    const std::string& base,
                    bool do_trace,
                    bool do_memdump,
                    bool do_before,
                    bool do_after,
                    bool do_step)
{
    /* open requested files */
    std::ofstream tf, bf, af;

    if (do_trace) {
        tf.open((base + ".trace").c_str());
        if (!tf) std::cerr << "Cannot create " << base << ".trace\n";
    }
    if (do_before) {
        bf.open((base + ".before").c_str());
        if (!bf) std::cerr << "Cannot create " << base << ".before\n";
    }
    if (do_after) {
        af.open((base + ".after").c_str());
        if (!af) std::cerr << "Cannot create " << base << ".after\n";
    }

    /* ---- write headers to files AND terminal ---- */
    auto emit = [](std::ofstream& f, const std::string& line) {
        std::cout << line << '\n';
        if (f.is_open()) f << line << '\n';
    };

    if (do_trace) {
        emit(tf, trace_header());
        emit(tf, divider_trace());
    }
    if (do_before) {
        emit(bf, "BEFORE - Register state before each instruction");
        emit(bf, reg_header());
        emit(bf, divider_reg());
    }
    if (do_after) {
        emit(af, "AFTER - Register state after each instruction");
        emit(af, reg_header());
        emit(af, divider_reg());
    }

    emu.reg[REG_PC] = 0;
    emu.reg[REG_SP] = 0;

    while (emu.running) {

        int pc = emu.reg[REG_PC];
        if (pc < 0 || pc >= MEMORY_SIZE) {
            std::cerr << "PC out of bounds: " << hex8(pc) << '\n';
            break;
        }

        int instr   =  emu.memory[pc];
        int opcode  =  instr & 0xFF;
        int operand = (instr >> 8) & 0xFFFFFF;
        if (operand & 0x800000)
            operand |= static_cast<int>(0xFF000000);

        int new_pc    = pc + 1;
        int a_before  = emu.reg[REG_A];
        int b_before  = emu.reg[REG_B];
        int sp_before = emu.reg[REG_SP];

        /* trace row — state BEFORE */
        if (do_trace) {
            std::string row = trace_row(emu.instruction_count,
                                        pc, instr, opcode, operand,
                                        a_before, b_before, sp_before);
            std::cout << row << '\n';
            if (tf.is_open()) {
                tf << row << '\n';
                tf.flush();
            }
        }

        /* before row */
        if (do_before) {
            std::string row = reg_row(emu.instruction_count,
                                      pc, a_before, b_before, sp_before);
            if (!do_trace) std::cout << row << '\n';
            if (bf.is_open()) bf << row << '\n';
        }

        if (do_step) {
            std::cout << "Press Enter to execute next instruction, or 'q' then Enter to quit..." << std::flush;
            std::string stepInput;
            if (!std::getline(std::cin, stepInput) || stepInput == "q" || stepInput == "Q") {
                std::cout << "\nExecution stopped by user.\n";
                emu.running = false;
                break;
            }
        }

        /* ---- execute instruction ---- */
        switch (opcode) {
            case 0:
                emu.reg[REG_B]  = emu.reg[REG_A];
                emu.reg[REG_A]  = operand;
                emu.reg[REG_PC] = new_pc;
                break;
            case 1:
                emu.reg[REG_A] += operand;
                emu.reg[REG_PC] = new_pc;
                break;
            case 2:
                emu.reg[REG_B]  = emu.reg[REG_A];
                emu.reg[REG_A]  = emu.memory[emu.reg[REG_SP] + operand];
                emu.reg[REG_PC] = new_pc;
                break;
            case 3:
                emu.memory[emu.reg[REG_SP] + operand] = emu.reg[REG_A];
                emu.reg[REG_A]  = emu.reg[REG_B];
                emu.reg[REG_PC] = new_pc;
                break;
            case 4:
                emu.reg[REG_A]  = emu.memory[emu.reg[REG_A] + operand];
                emu.reg[REG_PC] = new_pc;
                break;
            case 5:
                emu.memory[emu.reg[REG_A] + operand] = emu.reg[REG_B];
                emu.reg[REG_PC] = new_pc;
                break;
            case 6:
                emu.reg[REG_A]  = emu.reg[REG_B] + emu.reg[REG_A];
                emu.reg[REG_PC] = new_pc;
                break;
            case 7:
                emu.reg[REG_A]  = emu.reg[REG_B] - emu.reg[REG_A];
                emu.reg[REG_PC] = new_pc;
                break;
            case 8:
                emu.reg[REG_A]  = emu.reg[REG_B] << emu.reg[REG_A];
                emu.reg[REG_PC] = new_pc;
                break;
            case 9:
                emu.reg[REG_A]  = static_cast<int>(
                    static_cast<unsigned>(emu.reg[REG_B]) >> emu.reg[REG_A]);
                emu.reg[REG_PC] = new_pc;
                break;
            case 10:
                emu.reg[REG_SP] += operand;
                emu.reg[REG_PC]  = new_pc;
                break;
            case 11:
                emu.reg[REG_SP] = emu.reg[REG_A];
                emu.reg[REG_A]  = emu.reg[REG_B];
                emu.reg[REG_PC] = new_pc;
                break;
            case 12:
                emu.reg[REG_B]  = emu.reg[REG_A];
                emu.reg[REG_A]  = emu.reg[REG_SP];
                emu.reg[REG_PC] = new_pc;
                break;
            case 13:
                emu.reg[REG_B]  = emu.reg[REG_A];
                emu.reg[REG_A]  = new_pc;
                emu.reg[REG_PC] = new_pc + operand;
                break;
            case 14:
                emu.reg[REG_PC] = emu.reg[REG_A];
                emu.reg[REG_A]  = emu.reg[REG_B];
                break;
            case 15:
                emu.reg[REG_PC] = (emu.reg[REG_A] == 0)
                                  ? new_pc + operand : new_pc;
                break;
            case 16:
                emu.reg[REG_PC] = (emu.reg[REG_A] < 0)
                                  ? new_pc + operand : new_pc;
                break;
            case 17:
                emu.reg[REG_PC] = new_pc + operand;
                break;
            case 18:
                emu.running     = false;
                emu.reg[REG_PC] = new_pc;
                break;
            default:
                std::cerr << "Unknown opcode " << opcode
                          << " at PC " << hex8(pc) << '\n';
                emu.running = false;
                break;
        }

        /* after row */
        if (do_after) {
            std::string row = reg_row(emu.instruction_count,
                                      emu.reg[REG_PC],
                                      emu.reg[REG_A],
                                      emu.reg[REG_B],
                                      emu.reg[REG_SP]);
            if (!do_trace && !do_before) std::cout << row << '\n';
            if (af.is_open()) af << row << '\n';
        }

        ++emu.instruction_count;

        if (emu.instruction_count > 11000000) {
            std::cerr << "Possible infinite loop - stopping at PC "
                      << hex8(emu.reg[REG_PC]) << '\n';
            break;
        }
    }

    /* ---- footers ---- */
    std::string summary =
        "Instructions executed: " + std::to_string(emu.instruction_count);
    std::string rs = reg_state_block(emu);

    if (do_trace) {
        std::cout << divider_trace() << '\n' << summary << "\n\n" << rs;
        if (tf.is_open())
            tf << divider_trace() << '\n' << summary << "\n\n" << rs;
    }
    if (do_before) {
        if (bf.is_open())
            bf << divider_reg() << '\n' << summary << "\n\n" << rs;
        if (!do_trace)
            std::cout << divider_reg() << '\n' << summary << "\n\n" << rs;
    }
    if (do_after) {
        if (af.is_open())
            af << divider_reg() << '\n' << summary << "\n\n" << rs;
        if (!do_trace && !do_before)
            std::cout << divider_reg() << '\n' << summary << "\n\n" << rs;
    }

    /* ---- memory dump ---- */
    if (do_memdump) {
        std::string mpath = base + ".memdump";
        std::ofstream mf(mpath.c_str());
        if (mf) {
            write_memdump(emu, mf, emu.instruction_count);
            mf.close();
        } else {
            std::cerr << "Cannot create " << mpath << '\n';
        }
        std::cout << '\n';
        write_memdump(emu, std::cout, emu.instruction_count);
    }

    if (!do_trace && !do_before && !do_after)
        std::cout << '\n' << rs;

    /* ---- report ---- */
    std::cout << "\nFiles generated:\n";
    if (do_trace)   std::cout << "  " << base << ".trace\n";
    if (do_memdump) std::cout << "  " << base << ".memdump\n";
    if (do_before)  std::cout << "  " << base << ".before\n";
    if (do_after)   std::cout << "  " << base << ".after\n";
}


/*  Strip directory and extension                                       */

static std::string base_name(const std::string& path)
{
    std::string s = path;
    size_t slash = s.find_last_of("/\\");
    if (slash != std::string::npos) s = s.substr(slash + 1);
    size_t dot = s.rfind('.');
    if (dot != std::string::npos) s = s.substr(0, dot);
    return s;
}


/*  Parse -o flag                                                       */

static void parse_flags(int argc, char* argv[],
                        std::vector<std::string>& files,
                        bool& do_trace,
                        bool& do_memdump,
                        bool& do_before,
                        bool& do_after,
                        bool& do_step)
{
    do_trace = do_memdump = do_before = do_after = do_step = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-o" && i + 1 < argc) {
            std::string opts(argv[++i]);
            for (size_t j = 0; j < opts.size(); ++j) {
                if (opts[j] == '1') do_trace   = true;
                if (opts[j] == '2') do_memdump = true;
                if (opts[j] == '3') do_before  = true;
                if (opts[j] == '4') do_after   = true;
                if (opts[j] == 's' || opts[j] == 'S') do_step = true;
            }
        } else if (arg == "-s") {
            do_step = true;
        } else if (arg[0] != '-') {
            files.push_back(arg);
        }
    }

    /* default to trace if nothing chosen */
    if (!do_trace && !do_memdump && !do_before && !do_after) {
        std::cout << "No -o flag given. Defaulting to trace file.\n";
        do_trace = true;
    }
}


/*  main                                                                */

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "SIMPLEX Emulator\n\n";
        std::cout << "Usage: " << argv[0]
                  << " <file.o> [file2.o ...] -o <flags> [-s]\n\n";
        std::cout << "Output flags (combine freely, no spaces):\n";
        std::cout << "  1  Trace file    (.trace)   instruction trace\n";
        std::cout << "  2  Memory dump   (.memdump) 4 words per line\n";
        std::cout << "  3  Before file   (.before)  regs BEFORE each instruction\n";
        std::cout << "  4  After file    (.after)   regs AFTER  each instruction\n\n";
        std::cout << "Optional flags:\n";
        std::cout << "  -s  Interactive stepping mode (press Enter to execute each step)\n\n";
        std::cout << "Examples:\n";
        std::cout << "  " << argv[0] << " test4.o -o 1\n";
        std::cout << "  " << argv[0] << " test4.o -o 13\n";
        std::cout << "  " << argv[0] << " test4.o -o 1234\n";
        std::cout << "  " << argv[0] << " test4.o -o 1 -s\n";
        std::cout << "  " << argv[0] << " test1.o test4.o -o 24\n";
        return 1;
    }

    std::vector<std::string> files;
    bool do_trace, do_memdump, do_before, do_after, do_step;
    parse_flags(argc, argv, files, do_trace, do_memdump, do_before, do_after, do_step);

    if (files.empty()) {
        std::cerr << "No object files specified.\n";
        return 1;
    }

    /* show selected options */
    std::cout << "\nSelected output:\n";
    if (do_trace)   std::cout << "  [1] Trace file\n";
    if (do_memdump) std::cout << "  [2] Memory dump\n";
    if (do_before)  std::cout << "  [3] Before file\n";
    if (do_after)   std::cout << "  [4] After file\n";
    if (do_step)    std::cout << "  [s] Interactive stepping\n";

    for (size_t i = 0; i < files.size(); ++i) {
        std::string base = base_name(files[i]);
        std::cout << "\n========================================\n";
        std::cout << "  Processing: " << files[i] << "\n";
        std::cout << "========================================\n";
        Emulator emu;
        if (load_object(emu, files[i]))
            execute(emu, base, do_trace, do_memdump, do_before, do_after, do_step);
    }

    return 0;
}