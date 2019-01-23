#pragma once

#include "DoubleBuffer.h"
#include <Kernel/CharacterDevice.h>
#include <Kernel/UnixTypes.h>

class Process;

class TTY : public CharacterDevice {
public:
    virtual ~TTY() override;

    virtual ssize_t read(Process&, byte*, size_t) override;
    virtual ssize_t write(Process&, const byte*, size_t) override;
    virtual bool can_read(Process&) const override;
    virtual bool can_write(Process&) const override;
    virtual int ioctl(Process&, unsigned request, unsigned arg) override final;

    virtual String tty_name() const = 0;

    unsigned short rows() const { return m_rows; }
    unsigned short columns() const { return m_columns; }

    void set_pgid(pid_t pgid) { m_pgid = pgid; }
    pid_t pgid() const { return m_pgid; }

    void set_termios(const termios&);
    bool should_generate_signals() const { return m_termios.c_lflag & ISIG; }
    bool should_echo_input() const { return m_termios.c_lflag & ECHO; }
    bool in_canonical_mode() const { return m_termios.c_lflag & ICANON; }

    void set_default_termios();

protected:
    virtual void on_tty_write(const byte*, size_t) = 0;
    void set_size(unsigned short columns, unsigned short rows);

    TTY(unsigned major, unsigned minor);
    void emit(byte);

private:
    // ^CharacterDevice
    virtual bool is_tty() const final override { return true; }

    void generate_signal(int signal);

    DoubleBuffer m_buffer;
    pid_t m_pgid { 0 };
    termios m_termios;
    unsigned short m_rows { 0 };
    unsigned short m_columns { 0 };
};

