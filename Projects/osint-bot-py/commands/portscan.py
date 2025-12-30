import asyncio
import socket
import discord
from discord.ext import commands

class PortScanner(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    async def scan_port(self, host, port):
        """Scansiona una porta e prova a leggere il banner"""
        try:
            reader, writer = await asyncio.wait_for(
                asyncio.open_connection(host, port),
                timeout=1
            )

            # Prova a leggere un banner (servizio)
            try:
                banner = await asyncio.wait_for(reader.read(100), timeout=1)
                banner = banner.decode(errors="ignore").strip()
            except:
                banner = "Nessun banner"

            writer.close()
            await writer.wait_closed()

            return port, banner

        except:
            return None

    @commands.command()
    async def portscan(self, ctx, host: str, ports: str = None):
        """
        Scansiona porte comuni o un range personalizzato.
        Esempi:
        !portscan google.com
        !portscan 1.1.1.1 1-1000
        """

        await ctx.send(f"🔍 Scansione porte per: `{host}`")

        # Porte comuni se non specificato
        common_ports = [
            21, 22, 23, 25, 53, 80, 110, 143,
            443, 3306, 3389, 8080, 8443
        ]

        # Se l'utente specifica un range personalizzato
        if ports:
            try:
                start, end = ports.split("-")
                start = int(start)
                end = int(end)

                if start < 1 or end > 65535 or start >= end:
                    await ctx.send("❌ Range porte non valido.")
                    return

                port_list = list(range(start, end + 1))

            except:
                await ctx.send("❌ Usa il formato corretto: `1-1000`")
                return
        else:
            port_list = common_ports

        # Scansione asincrona
        tasks = [self.scan_port(host, port) for port in port_list]
        results = await asyncio.gather(*tasks)

        open_ports = [r for r in results if r]

        embed = discord.Embed(
            title=f"🧪 Port Scan — {host}",
            color=discord.Color.blue()
        )

        if open_ports:
            for port, banner in open_ports[:20]:  # Limite per evitare spam
                embed.add_field(
                    name=f"🟢 Porta {port} aperta",
                    value=f"Banner: `{banner}`",
                    inline=False
                )
        else:
            embed.add_field(
                name="🔴 Nessuna porta aperta trovata",
                value="Il server non espone servizi rilevabili.",
                inline=False
            )

        embed.set_footer(text="OSINT Port Scanner PRO")
        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(PortScanner(bot))